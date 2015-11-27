/*
Copyright: Chris Kellner 2015
License: MIT
*/
#include "Mustache.h"
#include "Common.h"
#include <list>
#include <regex>

namespace Mustache{
	typedef std::shared_ptr<AMustacheTemplate> MustachePtr;
	typedef std::vector<MustachePtr> MustacheCollection;
	
	Context::Context(JSONElement *_content, std::function<AMustacheTemplate*(std::string)> _templateFactory)
		:content(_content), templateFactory(_templateFactory), parent(nullptr){
	}
	
	Context::Context(Context *_parent, JSONElement *_content) 
		: content(_content), templateFactory(nullptr), parent(_parent){
	}

	Context::Context(Context &isolate){
		content = isolate.content;
		parent = nullptr;
		auto p = &isolate;
		while (p && !p->templateFactory){
			p = p->parent;
		}
		if (p){
			templateFactory = p->templateFactory;
		}
		else{
			templateFactory = nullptr;
		}
	}

	JSONElement* Context::find(std::string path){
		JSONElement* elem;
		Context* ctx = this;
		do{
			elem = ctx->content->find(path);
			ctx = ctx->parent;
		} while (ctx && (!elem || !*elem));
		return elem;
	}

	AMustacheTemplate* Context::findTemplate(std::string key){
		Context* ctx = this;
		std::function<AMustacheTemplate*(std::string)> factory;
		do{
			factory = ctx->templateFactory;
			ctx = ctx->parent;
		} while (ctx && !factory);

		if (factory){
			return factory(key);
		}

		return nullptr;
	}

	class LiteralTemplate : public AMustacheTemplate{
	private:
		std::string contents;
	public:
		LiteralTemplate(std::string _contents){
			contents = _contents;
		}

		// from: http://stackoverflow.com/questions/5343190/how-do-i-replace-all-instances-of-of-a-string-with-another-string
		void ReplaceStringInPlace(std::string& subject, const std::string& search,
			const std::string& replace) {
			size_t pos = 0;
			while ((pos = subject.find(search, pos)) != std::string::npos) {
				subject.replace(pos, search.length(), replace);
				pos += replace.length();
			}
		}

		std::string render(Context &ctx){
			auto c = contents;
			ReplaceStringInPlace(c, "\\r", "\r");
			ReplaceStringInPlace(c, "\\n", "\n");
			ReplaceStringInPlace(c, "\\t", "\t");
			return c;
		}
	};

	class VariableTemplate : public AMustacheTemplate{
	private:
		std::string key;
	public:
		VariableTemplate(std::string _key){
			key = _key;
		}

		std::string render(Context &ctx){
			return ctx.find(key)->toString();
		}
	};

	class CallTemplate : public AMustacheTemplate{
	private:
		std::string key;
		bool isolate;
	public:
		CallTemplate(std::string _key, bool _isolate){
			key = _key;
			isolate = _isolate;
		}

		std::string render(Context &ctx){
			auto tmpl = ctx.findTemplate(key);
			if (tmpl){
				if (isolate){
					Context isolate = Context(ctx);
					return tmpl->render(isolate);
				}
				else{
					return tmpl->render(ctx);
				}
			}
			return "";
		}
	};

	enum SectionType{
		Normal,
		Inverted,
		CompareEqual,
		CompareNotEqual,
		CompareGreater,
		CompareLess,
		InSet,
		NotInSet
	};

	class SectionTemplate : public AMustacheTemplate{
	public:
		std::string key;
		JSONString compVal;
		SectionType type;
		MustacheCollection contents;

		SectionTemplate(std::string _key, SectionType _type)
			:contents(),
			type(_type),
			compVal(""){
			key = _key;
			auto eqI = key.find_first_of("=<>!$^");
			if (eqI != std::string::npos){
				switch (key[eqI]){
				case '=':
					type = CompareEqual;
					break;
				case '!':
					type = CompareNotEqual;
					break;
				case '<':
					type = CompareLess;
					break;
				case '>':
					type = CompareGreater;
					break;
				case '$':
					type = InSet;
					break;
				case '^':
					type = NotInSet;
					break;
				}
				compVal = key.substr(eqI + 1);
				key = key.substr(0, eqI);
			}
		}

		~SectionTemplate(){
			contents.empty();
		}

		std::string render(Context &ctx){
			auto elem = ctx.find(key);
			if (elem && *elem){
				if (type != Inverted){
					switch (type){
					case CompareEqual:
						if (elem->toString().compare(compVal.toString()) != 0){
							return "";
						}
						break;
					case CompareNotEqual:
						if (elem->toString().compare(compVal.toString()) == 0){
							return "";
						}
						break;
					case CompareGreater:
						if (elem->toFloat() <= compVal.toFloat()){
							return "";
						}
						break;
					case CompareLess:
						if (elem->toFloat() >= compVal.toFloat()){
							return "";
						}
						break;
					case InSet:{
							std::vector<std::string> items;
							Tokenize(compVal.toString(), items, "|");
							if (std::find(items.begin(), items.end(), elem->toString()) == items.end()){
								return "";
							}
						}
						break;
					case NotInSet:{
							std::vector<std::string> items;
							Tokenize(compVal.toString(), items, "|");
							if (std::find(items.begin(), items.end(), elem->toString()) != items.end()){
								return "";
							}
						}
						break;
					case Normal:
					default:
						break;
					}
					if (elem->getType() == JSON_ARRAY){
						auto arr = (JSONArray*)elem;
						std::string buf;
						for (auto iter = arr->begin(); iter != arr->end(); iter++){
							Context child(&ctx, iter->get());
							for (auto cIter = contents.begin(); cIter != contents.end(); cIter++){
								buf.append((*cIter)->render(child));
							}
						}
						return buf;
					}
					else{
						Context child(&ctx, elem);
						std::string buf;
						for (auto cIter = contents.begin(); cIter != contents.end(); cIter++){
							buf.append((*cIter)->render(child));
						}
						return buf;
					}
				}
			}
			else if (type == Inverted){
				std::string buf;
				for (auto cIter = contents.begin(); cIter != contents.end(); cIter++){
					buf.append((*cIter)->render(ctx));
				}
				return buf;
			}

			return "";
		}
	};

	enum TokenType{
		Invalid = -1,
		Comment,
		Literal,
		Variable,
		SectionStart,
		InvertStart,
		SectionEnd,
		Partial,
		IsolatedPartial
	};

	struct Token {
		TokenType type;
		std::string content;

		Token(TokenType _type, std::string _content){
			type = _type;
			content = _content;
		}
	};

	class Tokenizer {
	public:
		Tokenizer()
		{
		}

		inline static TokenType parseTag(std::string open, std::string close, std::string &tag){
			// strip the markup
			tag = tag.substr(open.length(), tag.length() - (open.length() + close.length()));
			switch (tag[0]){
			case '#':
				tag = tag.substr(1);
				return SectionStart;
			case '!':
				tag = tag.substr(1);
				return Comment;
			case '^':
				tag = tag.substr(1);
				return InvertStart;
			case '/':
				tag = tag.substr(1);
				return SectionEnd;
			case '>':
				tag = tag.substr(1);
				if (tag[0] == '>'){
					// {{>>name}}
					tag = tag.substr(1);
					return IsolatedPartial;
				}

				return Partial;
			default:
				return Variable;
			}
		}

		void tokenize(std::string input, std::vector<Token> &tokens){
			unsigned int cpos = 0;
			std::string openStr = "{{";
			std::string closeStr = "}}";
			while (cpos < input.length()){
				// find a tag
				auto openPos = input.find(openStr, cpos);
				if (openPos != std::string::npos){
					if (cpos != openPos){
						tokens.push_back(Token(Literal, input.substr(cpos, openPos - cpos)));
					}
					cpos = openPos;

					auto closePos = input.find(closeStr, cpos);
					if (closePos != std::string::npos){
						closePos += closeStr.length();
						std::string tag = input.substr(openPos, closePos - openPos);
						cpos = closePos;
						
						TokenType type = parseTag(openStr, closeStr, tag);
						tokens.push_back(Token(type, tag));
					}
					else { // no more tags, consume the rest of the string
						tokens.push_back(Token(Literal, input.substr(cpos)));
						cpos = input.length();
					}
				}
				else { // no more tags, consume the rest of the string
					tokens.push_back(Token(Literal, input.substr(cpos)));
					cpos = input.length();
				}
			}
		}

		bool parse(std::vector<Token> &tokens, MustacheCollection &templates){
			std::stack<SectionTemplate*> sectionStack;

			for (auto it = tokens.begin(); it != tokens.end(); it++){
				MustacheCollection *t = &templates;
				if (!sectionStack.empty()){
					t = &sectionStack.top()->contents;
				}

				switch (it->type){
				case Literal:
					t->push_back(MustachePtr(new LiteralTemplate(it->content)));
					break;
				case Variable:
					t->push_back(MustachePtr(new VariableTemplate(it->content)));
					break;
				case SectionStart:{
						auto sec = new SectionTemplate(it->content, Normal);
						t->push_back(MustachePtr(sec));
						sectionStack.push(sec);
					}
					break;
				case InvertStart:{
					auto sec = new SectionTemplate(it->content, Inverted);
					t->push_back(MustachePtr(sec));
					sectionStack.push(sec);
					}
					break;
				case SectionEnd:
					if (sectionStack.top()->key.compare(it->content) == 0){
						sectionStack.pop();
					}
					else{
						return false;
					}
					break;
				case Partial:
					t->push_back(MustachePtr(new CallTemplate(it->content, false)));
					break;
				case IsolatedPartial:
					t->push_back(MustachePtr(new CallTemplate(it->content, true)));
					break;
				case Comment:
				default:
					break;
				}
			}

			return true;
		}
	};

	class RootTemplate : public AMustacheTemplate{
	private:
		MustacheCollection templs;
	public:
		RootTemplate(MustacheCollection _templs){
			templs = _templs;
		}

		~RootTemplate(){
			templs.empty();
		}

		std::string render(Context &ctx){
			std::string buf;
			for (auto iter = templs.begin(); iter != templs.end(); iter++){
				buf.append((*iter)->render(ctx));
			}
			return buf;
		};
	};

	Tokenizer tokenizer;
	std::string render(std::string templ, Context& context){

		std::vector<Token> tokens;
		tokenizer.tokenize(templ, tokens);

		MustacheCollection templs;
		tokenizer.parse(tokens, templs);

		RootTemplate t(templs);
		return t.render(context);
	}

	AMustacheTemplate *parse(std::string templ){
		std::vector<Token> tokens;
		tokenizer.tokenize(templ, tokens);

		MustacheCollection templs;
		if (tokenizer.parse(tokens, templs)){
			return new RootTemplate(templs);
		}

		return nullptr;
	}

	std::string renderTemplate(AMustacheTemplate *templ, Context& context){
		if (templ){
			return templ->render(context);
		}
		return "";
	}
}
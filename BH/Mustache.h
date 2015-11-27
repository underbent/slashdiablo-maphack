/*
Copyright: Chris Kellner 2015
License: MIT
*/
#pragma once
#include <string>
#include <functional>
#include "JSONObject.h"

/*
	Based on the spec located at: https://mustache.github.io/mustache.5.html
*/
namespace Mustache{
	class Context;

	class AMustacheTemplate{
	public:
		AMustacheTemplate(){}
		virtual ~AMustacheTemplate(){}

		virtual std::string render(Context &ctx){ return ""; };
	};

	class Context{
	private:
		Context *parent;
		JSONElement *content;
		std::function<AMustacheTemplate*(std::string)> templateFactory;

	public:
		Context(JSONElement *_content, std::function<AMustacheTemplate*(std::string)> _templateFactory);
		Context(Context *_parent, JSONElement *_content);
		Context(Context &isolate);
		
		JSONElement* find(std::string path);
		AMustacheTemplate* findTemplate(std::string key);
	};

	/*
		Parses the mustache template from the string.
		
		Returns: A pointer to a New mustache template.
		Caller must delete the returned pointer.
	*/
	AMustacheTemplate *parse(std::string templ);
	std::string render(std::string templ, Context& context);
	std::string renderTemplate(AMustacheTemplate *templ, Context& context);
}
/*
Copyright: Chris Kellner 2015
License: MIT
*/
#include "JSONObject.h"
#include <stdarg.h>
#include <assert.h>
#include <sstream>

void string_AppendFormat(std::string &buffer, const char* format, ...){
	char szBuffer[2048] = { 0 };
	va_list Args;
	va_start(Args, format);
	vsprintf_s(szBuffer, 2048, format, Args);
	va_end(Args);
	buffer.append(szBuffer);
}

std::string Json_Escape(const std::string &input){
	std::ostringstream ss;
	for (auto iter = input.cbegin(); iter != input.cend(); iter++) {
		switch (*iter) {
		case '\\': ss << "\\\\"; break;
		case '"': ss << "\\\""; break;
		case '/': ss << "\\/"; break;
		case '\b': ss << "\\b"; break;
		case '\f': ss << "\\f"; break;
		case '\n': ss << "\\n"; break;
		case '\r': ss << "\\r"; break;
		case '\t': ss << "\\t"; break;
		default: ss << *iter; break;
		}
	}
	return ss.str();
}

std::string Json_Unescape(const std::string &input){
	// TODO
	return input;
}

bool Json_ParseString(std::string &raw){
	if (raw.length() > 1){
		char quoteChar = raw[0];
		if (quoteChar == '"' || quoteChar == '\''){
			std::stringstream buf;
			if (raw[raw.length() - 1] == quoteChar){
				bool escape = false;
				for (unsigned int i = 1; i < raw.length() - 1; i++){
					if (raw[i] == quoteChar){
						if (escape){
							buf << quoteChar;
						}
						else{
							return false;
						}
					}
					else if (raw[i] == '\\'){
						escape = !escape;
						if (!escape){
							buf << '\\';
						}
					}
					else{
						buf << raw[i];
					}
				}

				raw.assign(buf.str());
				return true;
			}
		}

		return false;
	}

	return true;
}

void JSONWriter::writeStartArray(){
	if (_options == SER_OPT_FORMATTED && typeStack.top() == JSON_ARRAY){
		writeIndented("[");
	}
	else{
		writeRaw("[");
	}
	if (_options == SER_OPT_FORMATTED){
		indent++;
	}
	typeStack.push(JSON_ARRAY);
}

void JSONWriter::writeEndArray(){
	if (_options == SER_OPT_FORMATTED){
		indent--;
		writeNewlyIndented("]");
	}
	else{
		writeRaw("]");
	}
	assert(typeStack.top() == JSON_ARRAY);
	typeStack.pop();
}

void JSONWriter::writeStartObject(){
	if (_options == SER_OPT_FORMATTED && typeStack.top() == JSON_ARRAY){
		writeIndented("{");
	}
	else{
		writeRaw("{");
	}
	if (_options == SER_OPT_FORMATTED){
		indent++;
	}
	typeStack.push(JSON_OBJECT);
}

void JSONWriter::writeEndObject(){
	if (_options == SER_OPT_FORMATTED){
		indent--;
		writeNewlyIndented("}");
	}
	else{
		writeRaw("}");
	}
	assert(typeStack.top() == JSON_OBJECT);
	typeStack.pop();
}

void JSONWriter::writeKey(std::string key){
	if (_options == SER_OPT_FORMATTED){
		writeIndented("\"" + key + "\"");
	}
	else{
		writeRaw("\"" + key + "\"");
	}
}

void JSONWriter::writeValue(std::string value){
	if (_options == SER_OPT_FORMATTED && typeStack.top() == JSON_ARRAY){
		writeNewlyIndented(value);
	}
	else{
		writeRaw(value);
	}
}

void JSONWriter::writeRaw(std::string raw){
	_buffer.append(raw);
}

void JSONWriter::writeIndented(std::string raw){
	std::string buffer;
	string_AppendFormat(buffer, "%s%s", std::string(indent * 2, ' ').c_str(), raw.c_str());
	writeRaw(buffer);
}

void JSONWriter::writeNewlyIndented(std::string raw){
	std::string buffer;
	string_AppendFormat(buffer, "\n%s%s", std::string(indent * 2, ' ').c_str(), raw.c_str());
	writeRaw(buffer);
}

void JSONWriter::writeNext(){
	if (_options == SER_OPT_FORMATTED){
		writeRaw("\n");
	}
}

class JSONNull : public JSONElement {
public:
	JSONNull() { type = JSON_NULL; }
	virtual ~JSONNull() {}
	bool serialize(JSONWriter &writer) { writer.writeRaw("null"); return true; }
	bool hasValue() const { return false; }
	operator bool() const { return false; }
	operator int() const { return 0; }
	bool equals(JSONElement *other) { return !other || other->getType() == JSON_NULL; }
};

std::unique_ptr<JSONNull> jNull = std::unique_ptr<JSONNull>(new JSONNull());
inline JSONElement* JSONElement::Null(){
	return jNull.get();
}

JSONElement* JSONElement::find(std::string path) const{
	if (path.length() == 0 || path.compare("this") == 0){
		return (JSONElement*)this;
	}

	return JSONElement::Null();
}

JSONBool::JSONBool(bool value) :
	_value(value){
	type = JSON_BOOL;
}

bool JSONBool::serialize(JSONWriter &writer){
	if (_value){
		writer.writeValue(_value ? "true" : "false");
		return true;
	}
	return false;
}

JSONNumber::JSONNumber(float value) : _fvalue(value), _ivalue(0){
	type = JSON_NUMBER;
}

JSONNumber::JSONNumber(int value) : _ivalue(value), _fvalue(0){
	type = JSON_NUMBER;
}

bool JSONNumber::serialize(JSONWriter &writer){
	if (_ivalue){
		std::string v;
		string_AppendFormat(v, "%d", _ivalue);
		writer.writeValue(v);
		return true;
	}
	else if (_fvalue){
		std::string v;
		string_AppendFormat(v, "%f", _fvalue);
		writer.writeValue(v);
		return true;
	}
	return false;
}

std::string JSONNumber::toString() const{
	std::string buf;
	if (_ivalue){
		string_AppendFormat(buf, "%d", _ivalue);
	}
	else{
		string_AppendFormat(buf, "%f", _fvalue);
	}
	return buf;
}

JSONString::JSONString(std::string value) : _value(value){
	type = JSON_STRING;
}

bool JSONString::serialize(JSONWriter &writer){
	if (_value.length() > 0){
		std::string v;
		string_AppendFormat(v, "\"%s\"", Json_Escape(_value).c_str());
		writer.writeValue(v);
		return true;
	}
	return false;
}

bool JSONString::toBool() const{
	if (_value.compare("true") == 0){
		return true;
	}

	return false;
}
int JSONString::toInt() const{
	return atoi(_value.c_str());
}
float JSONString::toFloat() const{
	return (float)atof(_value.c_str());
}

JSONObject::~JSONObject(){
	_properties.clear();
}

void JSONObject::addEntry(std::string key, JSONElement* value){
	if (key.length() > 0 && value){
		_properties[key] = std::unique_ptr<JSONElement>(value);
	}
}

void JSONObject::set(std::string key, std::string value){
	addEntry(key, new JSONString(value));
}
void JSONObject::set(std::string key, bool value){
	addEntry(key, new JSONBool(value));
}
void JSONObject::set(std::string key, int value){
	addEntry(key, new JSONNumber(value));
}
void JSONObject::set(std::string key, float value){
	addEntry(key, new JSONNumber(value));
}
void JSONObject::set(std::string key, JSONObject* value){
	addEntry(key, value);
}
void JSONObject::set(std::string key, JSONArray* value){
	addEntry(key, value);
}

bool JSONObject::serialize(JSONWriter &writer){
	if (_properties.size() > 0){
		writer.writeStartObject();
		bool wroteOne = false;
		for (auto iter = _properties.begin(); iter != _properties.end(); iter++){
			if (iter->second && (*(iter->second)).hasValue()){
				if (wroteOne){
					writer.writeRaw(",");
				}
				writer.writeNext();
				writer.writeKey(iter->first);
				writer.writeRaw(": ");
				(*(iter->second)).serialize(writer);
				wroteOne = true;
			}
		}
		writer.writeEndObject();
		return true;
	}
	return false;
}

std::string JSONObject::getString(std::string key) const{
	auto entry = _properties.find(key);
	if (entry != _properties.end() && entry->second){
		return entry->second->toString();
	}
	return "";
}

float JSONObject::getNumber(std::string key) const{
	auto entry = _properties.find(key);
	if (entry != _properties.end() && entry->second){
		return entry->second->toFloat();
	}
	return 0;
}

bool JSONObject::getBool(std::string key) const{
	auto entry = _properties.find(key);
	if (entry != _properties.end() && entry->second){
		return entry->second->toBool();
	}

	return false;
}

JSONObject* JSONObject::getObject(std::string key) const{
	auto entry = _properties.find(key);
	if (entry != _properties.end() 
		&& entry->second && entry->second->getType() == JSON_OBJECT){
		return ((JSONObject*)entry->second.get());
	}

	return nullptr;
}

JSONArray* JSONObject::getArray(std::string key) const{
	auto entry = _properties.find(key);
	if (entry != _properties.end()
		&& entry->second && entry->second->getType() == JSON_ARRAY){
		return ((JSONArray*)entry->second.get());
	}

	return nullptr;
}

JSONElement* JSONObject::get(std::string key) const{
	if (key.compare("this") == 0){ return (JSONElement*)this; }
	auto entry = _properties.find(key);
	if (entry != _properties.end()){
		return entry->second.get();
	}

	return JSONElement::Null();
}

JSONElement* JSONObject::find(std::string path) const{
	auto item = JSONElement::find(path);
	if (!item || !*item){
		int idx = path.find_first_of(".[");
		if (idx > 0){
			auto key = path.substr(0, idx);
			auto elem = get(key);
			unsigned int nIdx = idx + (path[idx] == '.' ? 1 : 0);
			if (nIdx < path.length()){
				return elem->find(path.substr(nIdx));
			}
			return elem;
		}
		else if (idx == 0 && path[0] == '.'){
			return find(path.substr(1));
		}
		else if (idx == 0 && path[0] == '['){
			unsigned int eIdx = path.find_first_of(']', 1);
			if (eIdx > 0){
				auto key = path.substr(1, eIdx - 1);
				if (Json_ParseString(key)){
					auto elem = get(key);
					if (eIdx + 1 < path.length()){
						return elem->find(path.substr(eIdx + 1));
					}
					return elem;
				}
			}
		}
		else{
			return get(path);
		}
	}
	return item;
}

bool JSONObject::equals(JSONElement *other){
	if (other && other->getType() == JSON_OBJECT){
		JSONObject *o = (JSONObject*)other;
		if (_properties.size() == o->_properties.size()){
			for (auto it = _properties.begin(); it != _properties.end(); it++){
				if (!it->second->equals(o->get(it->first))){
					return false;
				}
			}
			return true;
		}
	}
	return false;
}

JSONArray::JSONArray() : _elements(){
	type = JSON_ARRAY;
}

JSONArray::~JSONArray(){
	_elements.clear();
}

bool JSONArray::serialize(JSONWriter &writer){
	if (_elements.size() > 0){
		writer.writeStartArray();
		bool wroteOne = false;
		for (auto iter = _elements.begin(); iter != _elements.end(); iter++){
			if (*iter && (*iter)->hasValue()){
				if (wroteOne){
					writer.writeRaw(",");
				}
				writer.writeNext();
				(*iter)->serialize(writer);
				wroteOne = true;
			}
		}
		writer.writeEndArray();
		return true;
	}
	return false; 
}

void JSONArray::addEntry(JSONElement* value){
	if (value){
		_elements.push_back(std::unique_ptr<JSONElement>(value));
	}
}

void JSONArray::add(std::string value){
	addEntry(new JSONString(value));
}
void JSONArray::add(bool value){
	addEntry(new JSONBool(value));
}
void JSONArray::add(int value){
	addEntry(new JSONNumber(value));
}
void JSONArray::add(float value){
	addEntry(new JSONNumber(value));
}
void JSONArray::add(JSONObject* value){
	addEntry(value);
}
void JSONArray::add(JSONArray* value){
	addEntry(value);
}

void JSONArray::remove(std::vector<std::unique_ptr<JSONElement>>::iterator elem){
	_elements.erase(elem);
}

void JSONArray::removeWhere(std::function<bool(JSONElement*)> predicate){
	auto iter = _elements.begin();
	while (iter != _elements.end()){
		if (predicate(iter->get())){
			iter = _elements.erase(iter);
		}
		else{
			iter++;
		}
	}
}

std::string JSONArray::getString(unsigned int index) const{
	if (index < _elements.size()){
		return _elements[index]->toString();
	}

	return "";
}

float JSONArray::getNumber(unsigned int index) const{
	if (index < _elements.size()){
		return _elements[index]->toFloat();
	}

	return 0;
}

bool JSONArray::getBool(unsigned int index) const{
	if (index < _elements.size()){
		return _elements[index]->toBool();
	}

	return false;
}

JSONObject* JSONArray::getObject(unsigned int index) const{
	if (index < _elements.size() && _elements[index]->getType() == JSON_OBJECT){
		return ((JSONObject*)_elements[index].get());
	}

	return nullptr;
}

JSONArray* JSONArray::getArray(unsigned int index) const{
	if (index < _elements.size() && _elements[index]->getType() == JSON_ARRAY){
		return ((JSONArray*)_elements[index].get());
	}

	return nullptr;
}

JSONElement* JSONArray::get(unsigned int index) const{
	if (index < _elements.size()){
		return _elements[index].get();
	}

	return JSONElement::Null();
}

bool isPositiveInteger(const std::string line)
{
	char* p;
	strtol(line.c_str(), &p, 10);
	return *p == 0;
}

JSONElement* JSONArray::find(std::string path) const{
	auto item = JSONElement::find(path);
	if (!item || !*item){
		int idx = path.find_first_of(".[");
		if (idx > 0){
			auto key = path.substr(0, idx);
			
			if (key.compare("this") == 0){
				return (JSONElement*)this;
			}
			else if (isPositiveInteger(key)){
				JSONElement* elem;
				auto index = atoi(key.c_str());
				elem = get(index);

				unsigned int nIdx = idx + (path[idx] == '.' ? 1 : 0);
				if (nIdx < path.length()){
					return elem->find(path.substr(nIdx));
				}
				return elem;
			}
		}
		else if (idx == 0 && path[0] == '.'){
			return find(path.substr(1));
		}
		else if (idx == 0 && path[0] == '['){
			unsigned int eIdx = path.find_first_of(']', 1);
			if (eIdx > 0){
				auto key = path.substr(1, eIdx - 1);
				if (isPositiveInteger(key)){
					auto index = atoi(key.c_str());
					auto elem = get(index);
					if (eIdx + 1 < path.length()){
						return elem->find(path.substr(eIdx + 1));
					}
					return elem;
				}
			}
		}
		else{
			if (path.compare("this") == 0){
				return (JSONElement*)this;
			}
			else{
				if (isPositiveInteger(path)){
					auto index = atoi(path.c_str());
					return get(index);
				}
			}
		}
	}
	return item;
}

// Searches through the array for an element equal to the specified target element
// Returns: The element contained in the array or null
JSONElement* JSONArray::contains(JSONElement* target) const {
	if (target){
		for (auto it = _elements.begin(); it != _elements.end(); it++){
			if (target->equals(it->get())){
				return it->get();
			}
		}
	}

	return NULL;
}

bool JSONArray::equals(JSONElement *other){
	if (other && other->getType() == JSON_ARRAY){
		JSONArray *o = (JSONArray*)other;
		if (_elements.size() == o->_elements.size()){
			for (int i = 0; i < _elements.size(); i++){
				if (!get(i)->equals(o->get(i))){
					return false;
				}
			}
			return true;
		}
	}
	return false;
}
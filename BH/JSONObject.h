/*
Copyright: Chris Kellner 2015
License: MIT
*/
#pragma once
#include <memory>
#include <vector>
#include <map>
#include <string>
#include <stack>
#include <functional>

enum JSONElementType{
	JSON_NULL,
	JSON_BOOL,
	JSON_NUMBER,
	JSON_STRING,
	JSON_OBJECT,
	JSON_ARRAY
};

enum SerializationOptions {
	SER_OPT_NONE = 0,
	SER_OPT_FORMATTED = 1
};

std::string Json_Escape(const std::string &input);
std::string Json_Unescape(const std::string &input);

class JSONWriter {
private:
	SerializationOptions _options;
	std::string &_buffer;
	int indent;
	std::stack<JSONElementType> typeStack;

	void writeIndented(std::string value);
	void writeNewlyIndented(std::string value);

public:
	JSONWriter(std::string &buffer, SerializationOptions options) :
		_buffer(buffer),
		_options(options),
		indent(0),
		typeStack(){
		typeStack.push(JSON_NULL);
	}

	SerializationOptions getOptions() { return _options; }

	void writeKey(std::string key);
	void writeValue(std::string value);
	void writeNext();
	void writeRaw(std::string value);
	void writeStartObject();
	void writeEndObject();
	void writeStartArray();
	void writeEndArray();
};

class JSONElement {
protected:
	JSONElementType type;
public:
	virtual ~JSONElement() {}

	JSONElementType getType() const { return type; }

	virtual bool toBool() const { return hasValue(); }
	virtual int toInt() const { return 0; }
	virtual float toFloat() const { return 0.0f; }
	virtual std::string toString() const { return ""; }
	 
	virtual bool serialize(JSONWriter &writer) = 0;
	virtual bool hasValue() const = 0;

	operator bool() const { return hasValue(); }

	virtual bool equals(JSONElement *other) = 0;

	/*
	Finds the child element within this element using the specified path.
	*/
	virtual JSONElement* find(std::string path) const;

	static JSONElement* Null();
};

class JSONBool : public JSONElement{
private:
	bool _value;
public:
	JSONBool(bool value);
	bool serialize(JSONWriter &writer);
	bool hasValue() const { return _value; }

	bool getValue() const { return _value; }

	bool toBool() const { return _value; }
	int toInt() const { return _value ? 1 : 0; }
	float toFloat() const { return _value ? 1.0f : 0.0f; }
	std::string toString() const { return _value ? "true" : "false"; }
	bool equals(JSONElement *other) { return other && other->getType() == JSON_BOOL && _value == ((JSONBool*)other)->_value; }
};

class JSONNumber : public JSONElement{
private:
	float _fvalue;
	int _ivalue;
public:
	JSONNumber(float value);
	JSONNumber(int value);
	bool serialize(JSONWriter &writer);
	bool hasValue() const { return _ivalue || _fvalue; }

	float getValue() const { return _ivalue ? _ivalue : _fvalue; }

	bool toBool() const { return hasValue(); }
	int toInt() const { return _ivalue ? _ivalue : (int)_fvalue; }
	float toFloat() const { return _fvalue ? _fvalue : (float)_ivalue; }
	std::string toString() const;
	bool equals(JSONElement *other) { return other && other->getType() == JSON_NUMBER && getValue() == ((JSONNumber*)other)->getValue(); }
};

class JSONString : public JSONElement{
private:
	std::string _value;
public:
	JSONString(std::string value);
	bool serialize(JSONWriter &writer);
	bool hasValue() const { return _value.length() > 0; }

	std::string getValue() const { return _value; }

	bool toBool() const;
	int toInt() const;
	float toFloat() const;
	std::string toString() const { return _value; }
	bool equals(JSONElement *other) { return other && other->getType() == JSON_STRING && getValue().compare(((JSONString*)other)->getValue()) == 0; }
};

class JSONArray;
class JSONObject : public JSONElement{
private:
	std::map<std::string, std::unique_ptr<JSONElement>> _properties;
protected:
	void addEntry(std::string key, JSONElement* value);
public:
	JSONObject() : _properties() {
		type = JSON_OBJECT;
	};

	~JSONObject();

	bool serialize(JSONWriter &writer);
	bool hasValue() const { return _properties.size() > 0; }
	int length() const { return _properties.size(); }

	JSONElement* get(std::string key) const;
	std::string getString(std::string key) const;
	float getNumber(std::string key) const;
	bool getBool(std::string key) const;
	JSONObject* getObject(std::string key) const;
	JSONArray* getArray(std::string key) const;

	void set(std::string key, std::string value);
	void set(std::string key, bool value);
	void set(std::string key, int value);
	void set(std::string key, float value);
	void set(std::string key, JSONObject* value);
	void set(std::string key, JSONArray* value);

	JSONElement* find(std::string path) const;

	bool equals(JSONElement *other);
};

class JSONArray : public JSONElement{
private:
	std::vector<std::unique_ptr<JSONElement>> _elements;
protected:
	void addEntry(JSONElement* value);
public:
	JSONArray();
	~JSONArray();
	bool serialize(JSONWriter &writer);
	bool hasValue() const { return _elements.size() > 0; }

	int length() const { return _elements.size(); }

	JSONElement* get(unsigned int index) const;
	std::string getString(unsigned int index) const;
	float getNumber(unsigned int index) const;
	bool getBool(unsigned int index) const;
	JSONObject* getObject(unsigned int index) const;
	JSONArray* getArray(unsigned int index) const;

	void add(std::string value);
	void add(bool value);
	void add(int value);
	void add(float value);
	void add(JSONObject* value);
	void add(JSONArray* value);

	void remove(std::vector<std::unique_ptr<JSONElement>>::iterator elem);
	void removeWhere(std::function<bool(JSONElement*)> predicate);

	std::vector<std::unique_ptr<JSONElement>>::iterator begin() { return _elements.begin(); }
	std::vector<std::unique_ptr<JSONElement>>::iterator end() { return _elements.end(); }

	JSONElement* find(std::string path) const;

	// Searches through the array for an element equal to the specified target element
	// Returns: The element contained in the array or null
	JSONElement* contains(JSONElement* target) const;

	bool equals(JSONElement *other);
};
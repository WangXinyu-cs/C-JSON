#ifndef _JSON_H_
#define _JSON_H_

#include <string>
#include <map>
#include <vector>

class JsonObject;
class JsonArray;

class Json
{
protected:
    Json() {};

public:
    virtual ~Json() {};
    virtual void serialize (std::string&) = 0;
    virtual void readable(int indent, std::string &ostr) = 0;
};

/**
 * when literal is given jsonstr is produced, and vice versa.
 * usually a user who construct a Json supply literal string,
 * a text from network or elsewhere contains rfc8259 sequence and goes into jsonstr.
*/
class JsonString : public Json
{
private:
    std::string m_literal;      // utf-8 string from user, maybe including escaped char
    std::string m_jsonstr;      // json string assumed obeying rfc8259 feed parser
    std::vector<std::string::size_type> m_errpos;

    void literal2jsonstr();
    void jsonstr2literal();

public:
    enum STR_TYPE {LITERAL, JSONSTR};

    JsonString();
    JsonString(std::string &str, STR_TYPE st);
    JsonString(const char *str, STR_TYPE st);
    JsonString(const JsonString &js);

    JsonString& operator=(const JsonString &js);

    const std::string& getLiteral();
    const std::string& getJsonstr();
    const std::vector<std::string::size_type>& getErrPos();
    void setLiteral(std::string &str);
    void setLiteral(const char *str);
    void setJsonstr(std::string &str);
    void setJsonstr(const char *str);

    virtual void serialize(std::string &ostr);
    virtual void readable(int indent, std::string &ostr);
};

class JsonInteger : public Json
{
private:
    long    m_value;

public:
    JsonInteger(long value);
    JsonInteger(const JsonInteger& value);

    JsonInteger& operator=(long value);
    JsonInteger& operator=(JsonInteger &value);

    long getValue();
    void setValue(long value);

    virtual void serialize(std::string &ostr);
    virtual void readable(int indent, std::string &ostr);
};

class JsonReal : public Json
{
private:
    double    m_value;

public:
    JsonReal(double value);
    JsonReal(const JsonReal& value);

    JsonReal& operator=(double value);
    JsonReal& operator=(JsonReal &value);

    double getValue();
    void setValue(double value);

    virtual void serialize(std::string &ostr);
    virtual void readable(int indent, std::string &ostr);
};

class JsonBool : public Json
{
private:
    bool    m_value;

public:
    JsonBool(bool value);
    JsonBool(const JsonBool& value);

    JsonBool& operator=(bool value);
    JsonBool& operator=(JsonBool &value);

    bool getValue();
    void setValue(bool value);

    virtual void serialize(std::string &ostr);
    virtual void readable(int indent, std::string &ostr);
};

class JsonNull : public Json
{
public:
    virtual void serialize(std::string &ostr) { ostr += "null"; };
    virtual void readable(int indent, std::string &ostr) { ostr += "null"; };
};

/**
 * if JsonObject and JsonArray should support copy and assign, the inside Json*
 * must be changed to shared_ptr<Json>
 * Never delete a Json* once it is inserted in a JsonObject or a JsonArray.
 * Only the outmost Json*, usually JsonObject or JsonArray, can be deleted.
*/
class JsonObject : public Json
{
private:
    std::map<std::string, Json*>   m_item;

public:
    ~JsonObject();

    // if the key exists, replace the old value with the new one
    JsonObject* add(std::string &name, long value);
    JsonObject* add(const char *name, long value);
    JsonObject* add(std::string &name, double value);
    JsonObject* add(const char *name, double value);
    JsonObject* add(std::string &name, bool value);
    JsonObject* add(const char *name, bool value);
    JsonObject* add(std::string &name, const char *value);
    JsonObject* add(const char *name, const char *value);
    JsonObject* add(std::string &name, std::string &value);
    JsonObject* add(const char *name, std::string &value);
    JsonObject* add(std::string &name, Json *value);
    JsonObject* add(const char *name, Json *value);

    // if the key exists the Json is deleted
    JsonObject* remove(std::string &name);
    JsonObject* remove(const char *name);

    /**
     * nullptr is returned when
     * either pos is out of range
     * or the element actual type is not consistent with what is claimed.
     */
    JsonString* getS(std::string &name);
    JsonString* getS(const char *name);
    JsonInteger* getI(std::string &name);
    JsonInteger* getI(const char *name);
    JsonReal* getR(std::string &name);
    JsonReal* getR(const char *name);
    JsonBool* getB(std::string &name);
    JsonBool* getB(const char *name);
    JsonNull* getN(std::string &name);
    JsonNull* getN(const char *name);
    JsonObject* getO(std::string &name);
    JsonObject* getO(const char *name);
    JsonArray* getA(std::string &name);
    JsonArray* getA(const char *name);

    virtual void serialize(std::string &ostr);
    virtual void readable(int indent, std::string &ostr);
};

class JsonArray : public Json
{
private:
    std::vector<Json*>    m_item;

public:
    ~JsonArray();

    int size();
    JsonArray* add(long value);
    JsonArray* add(double value);
    JsonArray* add(bool value);
    JsonArray* add(std::string &value);
    JsonArray* add(const char *value);
    JsonArray* add(Json *value);

    // if pos is out of range nothing is done
    JsonArray* remove(unsigned int pos);

    /**
     * nullptr is returned when
     * either pos is out of range
     * or the element actual type is not consistent with what is claimed.
     */
    JsonString* getS(unsigned int pos);
    JsonInteger* getI(unsigned int pos);
    JsonReal* getR(unsigned int pos);
    JsonBool* getB(unsigned int pos);
    JsonNull* getN(unsigned int pos);
    JsonObject* getO(unsigned int pos);
    JsonArray* getA(unsigned int pos);

    virtual void serialize (std::string &ostr);
    virtual void readable(int indent, std::string &ostr);
};

/**
 * down cast a Json to a concrete type,
 * NULL is returned when the transform fail.
*/
JsonString* asString(Json *json);
JsonInteger* asInteger(Json *json);
JsonReal* asReal(Json *json);
JsonBool* asBool(Json *json);
JsonNull* asNull(Json *json);
JsonObject* asObject(Json *json);
JsonArray* asArray(Json *json);

/**
 * parse json text and return the Json, NULL is returned when failed.
 * the input text is expected to follow rfc8259
*/
Json* parseJson(const char *text, std::string &err);
Json* parseJson(std::string &text, std::string &err);

/**
 * set precision for numbers
*/
void setPrecision(unsigned int pre);

#endif
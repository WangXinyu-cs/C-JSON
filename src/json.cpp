#include <cstdio>
#include <cstdlib>
#include <limits>
#include <cctype>
#include <clocale>
#include "parse.h"


using namespace std;
static const int BUF_LEN = 100;
static unsigned int PRECISION = 6;
static const string INDENT(4, ' ');

/**
 * JsonString Implementation
*/
JsonString::JsonString()
{}

JsonString::JsonString(std::string &str, STR_TYPE st)
{
    if (st == LITERAL)
    {
        m_literal = str;
        literal2jsonstr();
    }
    else
    {
        m_jsonstr = str;
        jsonstr2literal();
    }
}

JsonString::JsonString(const char *str, STR_TYPE st)
{
    if (!str || !*str)
        m_literal = m_jsonstr = "";
    else
    {
        if (st == LITERAL)
        {
            m_literal = str;
            literal2jsonstr();
        }
        else
        {
            m_jsonstr = str;
            jsonstr2literal();
        }
    }
}

JsonString::JsonString(const JsonString &js)
{
    m_literal = js.m_literal;
    m_jsonstr = js.m_jsonstr;
    m_errpos = js.m_errpos;
}

JsonString& JsonString::operator=(const JsonString &js)
{
    m_literal = js.m_literal;
    m_jsonstr = js.m_jsonstr;
    m_errpos = js.m_errpos;
    return *this;
}

const string& JsonString::getLiteral()
{
    return m_literal;
}

const string& JsonString::getJsonstr()
{
    return m_jsonstr;
}

const vector<string::size_type>& JsonString::getErrPos()
{
    return m_errpos;
}

void JsonString::setLiteral(string &str)
{
    m_literal = str;
    literal2jsonstr();
}

void JsonString::setLiteral(const char *str)
{
    if (str)
    {
        m_literal = str;
        literal2jsonstr();
    }
}

void JsonString::setJsonstr(string &str)
{
    m_jsonstr = str;
    jsonstr2literal();
}
    
void JsonString::setJsonstr(const char *str)
{
    if (str)
    {
        m_jsonstr = str;
        jsonstr2literal();
    }
}

void JsonString::serialize(std::string &ostr)
{
    ostr += "\"" + m_jsonstr + "\"";
}

void JsonString::readable(int indent, string &ostr)
{
    ostr += "\"" + m_literal + "\"";
}

void JsonString::literal2jsonstr()
{
    m_jsonstr.clear();
    m_errpos.clear();

    for (int i = 0; i < m_literal.size(); ++i)
    {
        if (m_literal[i] == '"')
        {
            m_jsonstr.push_back('\\');
            m_jsonstr.push_back('"');
        }
        else if (m_literal[i] == '\\')
        {
            m_jsonstr.push_back('\\');
            m_jsonstr.push_back('\\');
        }
        else if (m_literal[i] == '/')
        {
            m_jsonstr.push_back('\\');
            m_jsonstr.push_back('/');
        }
        else if (m_literal[i] == 0x08)
        {
            m_jsonstr.push_back('\\');
            m_jsonstr.push_back('b');
        }
        else if (m_literal[i] == 0x0c)
        {
            m_jsonstr.push_back('\\');
            m_jsonstr.push_back('f');
        }
        else if (m_literal[i] == 0x0a)
        {
            m_jsonstr.push_back('\\');
            m_jsonstr.push_back('n');
        }
        else if (m_literal[i] == 0x0d)
        {
            m_jsonstr.push_back('\\');
            m_jsonstr.push_back('r');
        }
        else if (m_literal[i] == 0x09)
        {
            m_jsonstr.push_back('\\');
            m_jsonstr.push_back('t');
        }
        else
            m_jsonstr.push_back(m_literal[i]);
    }
}

void JsonString::jsonstr2literal()
{
    m_literal.clear();
    m_errpos.clear();

    for (int i = 0; i < m_jsonstr.size();)
    {
        if (m_jsonstr[i] == '\\')
        {
            if (i + 1 < m_jsonstr.size())
            {
                if (m_jsonstr[i + 1] == '"')
                {
                    m_literal.push_back('"');
                    i += 2;
                }
                else if (m_jsonstr[i + 1] == '\\')
                {
                    m_literal.push_back('\\');
                    i += 2;
                }
                else if (m_jsonstr[i + 1] == '/')
                {
                    m_literal.push_back('/');
                    i += 2;
                }
                else if (m_jsonstr[i + 1] == 'b')
                {
                    m_literal.push_back(0x08);
                    i += 2;
                }
                else if (m_jsonstr[i + 1] == 'f')
                {
                    m_literal.push_back(0x0c);
                    i += 2;
                }
                else if (m_jsonstr[i + 1] == 'n')
                {
                    m_literal.push_back(0x0a);
                    i += 2;
                }
                else if (m_jsonstr[i + 1] == 'r')
                {
                    m_literal.push_back(0x0d);
                    i += 2;
                }
                else if (m_jsonstr[i + 1] == 't')
                {
                    m_literal.push_back(0x09);
                    i += 2;
                }
                else if (m_jsonstr[i + 1] == 'u')
                {
                    if (i + 5 >= m_jsonstr.size())
                    {
                        m_errpos.push_back(i);
                        m_literal.append(m_jsonstr, i, string::npos);
                        i = m_jsonstr.size();
                    }
                    else
                    {
                        int j = 2;
                        while (isxdigit(m_jsonstr[i + j]) && ++j <= 5);
                        if (j == 6)     // get 4 hex digits and convert to utf-8
                        {
                            setlocale(LC_CTYPE, "");
                            char buf[10] = {m_jsonstr[i + 2], m_jsonstr[i + 3], m_jsonstr[i + 4], m_jsonstr[i + 5], 0};
                            int len = wctomb(buf, strtoul(buf, 0, 16));
                            if (len != -1)  // convert OK
                            {
                                buf[len] = 0;
                                m_literal.append(buf, len);
                            }
                            else
                            {
                                m_errpos.push_back(i + 2);
                                m_literal.append(m_jsonstr, i, j);
                            }
                        }
                        else    // m_jsonstr[i + j] is not a hex digit
                        {
                            m_errpos.push_back(i + j);
                            m_literal.append(m_jsonstr, i, j);
                        }
                        i += j;
                    }
                }
                else
                {
                    m_errpos.push_back(i + 1);
                    m_literal.append(m_jsonstr, i, 2);
                    i += 2;
                }
            }
            else    // m_jsonstr end with '\'
            {
                m_errpos.push_back(i);
                m_literal.push_back('\\');
                ++i;
            }
        }
        else
            m_literal.push_back(m_jsonstr[i++]);
    }
}

/**
 * JsonInteger Implementation
*/
JsonInteger::JsonInteger(long value) : m_value(value) {}
JsonInteger::JsonInteger(const JsonInteger& value) : m_value(value.m_value) {}

JsonInteger& JsonInteger::operator=(long value)
{
    m_value = value;
    return *this;
}

JsonInteger& JsonInteger::operator=(JsonInteger &value)
{
    m_value = value.m_value;
    return *this;
}

long JsonInteger::getValue()
{
    return m_value;
}

void JsonInteger::setValue(long value)
{
    m_value = value;
}

void JsonInteger::serialize(string &ostr)
{
    char buf[BUF_LEN];
    snprintf(buf, BUF_LEN, "%ld", m_value);
    ostr += buf;
}

void JsonInteger::readable(int indent, string &ostr)
{
    serialize(ostr);
}

/**
 * JsonReal Implementation
*/
JsonReal::JsonReal(double value) : m_value(value) {}
JsonReal::JsonReal(const JsonReal &value) : m_value(value.m_value) {}

JsonReal& JsonReal::operator=(double value)
{
    m_value = value;
    return *this;
}

JsonReal& JsonReal::operator=(JsonReal &value)
{
    m_value = value.m_value;
    return *this;
}

double JsonReal::getValue()
{
    return m_value;
}

void JsonReal::setValue(double value)
{
    m_value = value;
}

void JsonReal::serialize(string &ostr)
{
    char buf[BUF_LEN];
    snprintf(buf, BUF_LEN, "%.*g", PRECISION, m_value);
    ostr += buf;
}

void JsonReal::readable(int indent, string &ostr)
{
    serialize(ostr);
}

/**
 * JsonBool Implementation
*/
JsonBool::JsonBool(bool value) : m_value(value) {}
JsonBool::JsonBool(const JsonBool &value) : m_value(value.m_value) {}

JsonBool& JsonBool::operator=(bool value)
{
    m_value = value;
    return *this;
}

JsonBool& JsonBool::operator=(JsonBool &value)
{
    m_value = value.m_value;
    return *this;
}

bool JsonBool::getValue()
{
    return m_value;
}

void JsonBool::setValue(bool value)
{
    m_value = value;
}

void JsonBool::serialize(string &ostr)
{
    ostr += m_value ? "true" : "false";
}

void JsonBool::readable(int indent, string &ostr)
{
    ostr += m_value ? "true" : "false";
}

/**
 * JsonObject Implementation
*/
JsonObject::~JsonObject()
{
    for (auto ite : m_item)
        delete ite.second;
    m_item.clear();
}

JsonObject* JsonObject::add(string &name, long value)
{
    auto ite = m_item.find(name);
    if (ite != m_item.end())
        delete ite->second;
    m_item[name] = new JsonInteger(value);
    return this;
}

JsonObject* JsonObject::add(const char *name, long value)
{
    if (!name)
        return this;

    auto ite = m_item.find(name);
    if (ite != m_item.end())
        delete ite->second;
    m_item[name] = new JsonInteger(value);
    return this;
}

JsonObject* JsonObject::add(string &name, double value)
{
    auto ite = m_item.find(name);
    if (ite != m_item.end())
        delete ite->second;
    m_item[name] = new JsonReal(value);
    return this;
}

JsonObject* JsonObject::add(const char *name, double value)
{
    if (!name)
        return this;

    auto ite = m_item.find(name);
    if (ite != m_item.end())
        delete ite->second;
    m_item[name] = new JsonReal(value);
    return this;
}

JsonObject* JsonObject::add(string &name, bool value)
{
    auto ite = m_item.find(name);
    if (ite != m_item.end())
        delete ite->second;
    m_item[name] = new JsonBool(value);
    return this;
}

JsonObject* JsonObject::add(const char *name, bool value)
{
    if (!name)
        return this;

    auto ite = m_item.find(name);
    if (ite != m_item.end())
        delete ite->second;
    m_item[name] = new JsonBool(value);
    return this;
}

JsonObject* JsonObject::add(string &name, const char *value)
{
    auto ite = m_item.find(name);
    if (ite != m_item.end())
        delete ite->second;

    if (value)
        m_item[name] = new JsonString(value, JsonString::LITERAL);
    else
        m_item[name] = new JsonNull();
    return this;
}

JsonObject* JsonObject::add(const char *name, const char *value)
{
    if (!name)
        return this;

    auto ite = m_item.find(name);
    if (ite != m_item.end())
        delete ite->second;

    if (value)
        m_item[name] = new JsonString(value, JsonString::LITERAL);
    else
        m_item[name] = new JsonNull();
    return this;
}

JsonObject* JsonObject::add(string &name, string &value)
{
    auto ite = m_item.find(name);
    if (ite != m_item.end())
        delete ite->second;
    m_item[name] = new JsonString(value, JsonString::LITERAL);
    return this;
}

JsonObject* JsonObject::add(const char *name, string &value)
{
    if (!name)
        return this;

    auto ite = m_item.find(name);
    if (ite != m_item.end())
        delete ite->second;
    m_item[name] = new JsonString(value, JsonString::LITERAL);
    return this;
}

JsonObject* JsonObject::add(string &name, Json *value)
{
    if (!value)
        value = new JsonNull();

    auto ite = m_item.find(name);
    if (ite != m_item.end())
        delete ite->second;
    m_item[name] = value;
    return this;
}

JsonObject* JsonObject::add(const char *name, Json *value)
{
    if (!name)
        return this;

    if (!value)
        value = new JsonNull();

    auto ite = m_item.find(name);
    if (ite != m_item.end())
        delete ite->second;
    m_item[name] = value;
    return this;
}

JsonObject* JsonObject::remove(string &name)
{
    auto ite = m_item.find(name);
    if (ite != m_item.end())
    {
        delete ite->second;
        m_item.erase(ite);
    }
    return this;
}

JsonObject* JsonObject::remove(const char *name)
{
    if (!name)
        return this;

    auto ite = m_item.find(name);
    if (ite != m_item.end())
    {
        delete ite->second;
        m_item.erase(ite);
    }
    return this;
}

JsonString* JsonObject::getS(std::string &name)
{
    auto ite = m_item.find(name);
    return ite != m_item.end() ? dynamic_cast<JsonString*>(ite->second) : nullptr;
}

JsonString* JsonObject::getS(const char *name)
{
    if (!name)
        return nullptr;

    auto ite = m_item.find(name);
    return ite != m_item.end() ? dynamic_cast<JsonString*>(ite->second) : nullptr;
}

JsonInteger* JsonObject::getI(std::string &name)
{
    auto ite = m_item.find(name);
    return ite != m_item.end() ? dynamic_cast<JsonInteger*>(ite->second) : nullptr;
}

JsonInteger* JsonObject::getI(const char *name)
{
    if (!name)
        return nullptr;

    auto ite = m_item.find(name);
    return ite != m_item.end() ? dynamic_cast<JsonInteger*>(ite->second) : nullptr;
}

JsonReal* JsonObject::getR(std::string &name)
{
    auto ite = m_item.find(name);
    return ite != m_item.end() ? dynamic_cast<JsonReal*>(ite->second) : nullptr;
}

JsonReal* JsonObject::getR(const char *name)
{
    if (!name)
        return nullptr;

    auto ite = m_item.find(name);
    return ite != m_item.end() ? dynamic_cast<JsonReal*>(ite->second) : nullptr;
}

JsonBool* JsonObject::getB(std::string &name)
{
    auto ite = m_item.find(name);
    return ite != m_item.end() ? dynamic_cast<JsonBool*>(ite->second) : nullptr;
}

JsonBool* JsonObject::getB(const char *name)
{
    if (!name)
        return nullptr;

    auto ite = m_item.find(name);
    return ite != m_item.end() ? dynamic_cast<JsonBool*>(ite->second) : nullptr;
}

JsonNull* JsonObject::getN(std::string &name)
{
    auto ite = m_item.find(name);
    return ite != m_item.end() ? dynamic_cast<JsonNull*>(ite->second) : nullptr;
}

JsonNull* JsonObject::getN(const char *name)
{
    if (!name)
        return nullptr;

    auto ite = m_item.find(name);
    return ite != m_item.end() ? dynamic_cast<JsonNull*>(ite->second) : nullptr;
}

JsonObject* JsonObject::getO(std::string &name)
{
    auto ite = m_item.find(name);
    return ite != m_item.end() ? dynamic_cast<JsonObject*>(ite->second) : nullptr;
}

JsonObject* JsonObject::getO(const char *name)
{
    if (!name)
        return nullptr;

    auto ite = m_item.find(name);
    return ite != m_item.end() ? dynamic_cast<JsonObject*>(ite->second) : nullptr;
}

JsonArray* JsonObject::getA(std::string &name)
{
    auto ite = m_item.find(name);
    return ite != m_item.end() ? dynamic_cast<JsonArray*>(ite->second) : nullptr;
}

JsonArray* JsonObject::getA(const char *name)
{
    if (!name)
        return nullptr;

    auto ite = m_item.find(name);
    return ite != m_item.end() ? dynamic_cast<JsonArray*>(ite->second) : nullptr;
}

void JsonObject::serialize(string &ostr)
{
    ostr += "{";
    for (auto ite : m_item)
    {
        ostr += "\"" + ite.first + "\":";
        ite.second->serialize(ostr);
        ostr += ",";
    }
    if (m_item.size())
        ostr.back() = '}';
    else
        ostr += "}";
}

void JsonObject::readable(int indent, string &ostr)
{
    ostr += "{\n";
    for (map<string, Json*>::iterator ite  = m_item.begin(); ite != m_item.end(); ++ite)
    {
        for (int i = 0; i <= indent; ++i) ostr += INDENT;
        ostr += "\"" + ite->first + "\": ";
        ite->second->readable(indent + 1, ostr);
        if (m_item.rbegin()->first != ite->first)
            ostr += ",";
        ostr += "\n";
    }
    for (int i = 0; i < indent; ++i) ostr += INDENT;
        ostr += "}";
}

/**
 * JsonArray Implementation
*/
JsonArray::~JsonArray()
{
    for (auto ite : m_item)
        delete ite;
    m_item.clear();
}

int JsonArray::size()
{
    return m_item.size();
}

JsonArray* JsonArray::add(long value)
{
    m_item.push_back(new JsonInteger(value));
    return this;
}

JsonArray* JsonArray::add(double value)
{
    m_item.push_back(new JsonReal(value));
    return this;
}

JsonArray* JsonArray::add(bool value)
{
    m_item.push_back(new JsonBool(value));
    return this;
}

JsonArray* JsonArray::add(string &value)
{
    m_item.push_back(new JsonString(value, JsonString::LITERAL));
    return this;
}

JsonArray* JsonArray::add(const char *value)
{
    if (value)
        m_item.push_back(new JsonString(value, JsonString::LITERAL));
    else
        m_item.push_back(new JsonNull());
    return this;
}

JsonArray* JsonArray::add(Json *value)
{
    if (!value)
        value = new JsonNull();

    m_item.push_back(value);
    return this;
}

JsonArray* JsonArray::remove(unsigned int pos)
{
    if (pos >= m_item.size())
        return this;

    delete m_item[pos];
    m_item.erase(m_item.begin() + pos);
    return this;
}

JsonString* JsonArray::getS(unsigned int pos)
{
    return pos < m_item.size() ? dynamic_cast<JsonString*>(m_item[pos]) : nullptr;
}

JsonInteger* JsonArray::getI(unsigned int pos)
{
    return pos < m_item.size() ? dynamic_cast<JsonInteger*>(m_item[pos]) : nullptr;
}

JsonReal* JsonArray::getR(unsigned int pos)
{
    return pos < m_item.size() ? dynamic_cast<JsonReal*>(m_item[pos]) : nullptr;
}

JsonBool* JsonArray::getB(unsigned int pos)
{
    return pos < m_item.size() ? dynamic_cast<JsonBool*>(m_item[pos]) : nullptr;
}

JsonNull* JsonArray::getN(unsigned int pos)
{
    return pos < m_item.size() ? dynamic_cast<JsonNull*>(m_item[pos]) : nullptr;
}

JsonObject* JsonArray::getO(unsigned int pos)
{
    return pos < m_item.size() ? dynamic_cast<JsonObject*>(m_item[pos]) : nullptr;
}

JsonArray* JsonArray::getA(unsigned int pos)
{
    return pos < m_item.size() ? dynamic_cast<JsonArray*>(m_item[pos]) : nullptr;
}

void JsonArray::serialize(string &ostr)
{
    ostr += "[";
    for (auto ite : m_item)
    {
        ite->serialize(ostr);
        ostr += ",";
    }
    if (m_item.size())
        ostr.back() = ']';
    else
        ostr += "]";
}

void JsonArray::readable(int indent, string &ostr)
{
    ostr += "[\n";
    for (int k = 0; k < m_item.size(); ++k)
    {
        for (int i = 0; i <= indent; ++i) ostr += INDENT;
        m_item[k]->readable(indent + 1, ostr);
        if (k != m_item.size() - 1)
            ostr += ",";
        ostr += "\n";
    }
    for (int i = 0; i < indent; ++i) ostr += INDENT;
        ostr += "]";
}

JsonString* asString(Json *json)
{
    return dynamic_cast<JsonString*>(json);
}

JsonInteger* asInteger(Json *json)
{
    return dynamic_cast<JsonInteger*>(json);
}

JsonReal* asReal(Json *json)
{
    return dynamic_cast<JsonReal*>(json);
}

JsonBool* asBool(Json *json)
{
    return dynamic_cast<JsonBool*>(json);
}

JsonNull* asnullptr(Json *json)
{
    return dynamic_cast<JsonNull*>(json);
}

JsonObject* asObject(Json *json)
{
    return dynamic_cast<JsonObject*>(json);
}

JsonArray* asArray(Json *json)
{
    return dynamic_cast<JsonArray*>(json);
}

Json* parseJson(const char *text, string &err)
{
    Json *json = nullptr;

    if (!text || !*text)
        return nullptr;

    yyscan_t scan;
    if (yylex_init(&scan))
    {
        perror("yylex_init fail.");
        return nullptr;
    }

    YY_BUFFER_STATE bs = yy_scan_string(text, scan);
    yyparse(json, err, scan);
    yy_delete_buffer(bs, scan);
    yylex_destroy(scan);
    return json;
}

Json* parseJson(string &text, string &err)
{
    return parseJson(text.c_str(), err);
}

void setPrecision(unsigned int pre)
{
    PRECISION = pre > numeric_limits<long double>::digits10 ? 
        numeric_limits<long double>::digits10 : pre;
}

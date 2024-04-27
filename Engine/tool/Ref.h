#pragma once

#include <string>
#include "String.h"

class ValueRef
{
public:
	ValueRef()
	{
		m_floatFormat = "%.1f";

		m_value_float = nullptr;
		m_value_int = nullptr;
		m_value_string = nullptr;
		m_typeInfo = nullptr;
	}

	virtual void setValueRefLink(const type_info* typeInfo, void* value)
	{
		m_typeInfo = typeInfo;

		std::string typeName = std::string(m_typeInfo->name());
		if (typeName.compare("float") == 0) {
			m_value_float = (float*)value;
		}
		else if (typeName.compare("int") == 0) {
			m_value_int = (int*)value;
		}
		else if (String::search(typeName, "std::basic_string")) {
			m_value_string = (std::string*)value;
		}
	}

	void setValueRef(std::string value)
	{
		if (m_typeInfo && String::trim(value).size() > 0) {
			std::string typeName = std::string(m_typeInfo->name());
			if (typeName.compare("float") == 0) {
				*m_value_float = std::stof(value);
			}
			else if (typeName.compare("int") == 0) {
				*m_value_int = std::stoi(value);
			}
			else if (String::search(typeName, "std::basic_string")) {
				*m_value_string = value;
			}
		}
	}

	std::string getValueRef()
	{
		std::string typeName = std::string(m_typeInfo->name());
		if (typeName.compare("float") == 0 && m_value_float != nullptr) {
			return String::ssprintf(&m_floatFormat[0], *m_value_float);
		}
		else if (typeName.compare("int") == 0 && m_value_int != nullptr) {
			return std::to_string(*m_value_int);
		}
		else if (String::search(typeName, "std::basic_string") && m_value_string != nullptr) {
			return *m_value_string;
		}

		return "";
	}

	bool isChangeValueRef(std::string value)
	{
		return m_typeInfo && getValueRef() != value;
	}

protected:
	std::string m_floatFormat;

    const type_info* m_typeInfo;
    float* m_value_float;
    int* m_value_int;
    std::string* m_value_string;
};
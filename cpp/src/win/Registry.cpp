#include "srl/win/Registry.h"

using namespace SRL;
using namespace SRL::Windows;

bool Registry::HasKey(HKEY root_key, const String& key)
{
	HKEY h_key;
	LONG result = ::RegOpenKeyEx(root_key,
					key.text(),
					0,
					KEY_QUERY_VALUE,
					&h_key);
	if (result == ERROR_SUCCESS)
	{
		::RegCloseKey(h_key);
		return true;
	}
	return false;
}

bool Registry::GetValue(HKEY root_key, const String& key, const String& name, String& value)
{
	HKEY h_key;
	LONG result = ::RegOpenKeyEx(root_key,
					key.text(),
					0,
					KEY_QUERY_VALUE,
					&h_key);
	if (result != ERROR_SUCCESS)
		return false;
	DWORD key_type = 0, value_size = 256;
	value.reserve(value_size);

	result = ::RegQueryValueEx(
					h_key, TEXT(name.text()),
					NULL, (LPDWORD)&key_type,
					(LPBYTE)value._bytes(), &value_size);
	
	value.resize(value_size);
	if (result != ERROR_SUCCESS)
		return false;

	::RegCloseKey(h_key);
	return true;
}

bool Registry::GetValue(HKEY root_key, const String& key, const String& name, int32& value)
{
	HKEY h_key;
	LONG result = ::RegOpenKeyEx(root_key,
					key.text(),
					0,
					KEY_QUERY_VALUE,
					&h_key);
	if (result != ERROR_SUCCESS)
		return false;
	DWORD key_type = 0, value_size = sizeof(value);

	result = ::RegQueryValueEx(
					h_key, TEXT(name.text()),
					NULL, (LPDWORD)&key_type,
					(LPBYTE)&value, &value_size);
	if (result != ERROR_SUCCESS)
		return false;

	::RegCloseKey(h_key);
	return true;
}

bool Registry::CreateKey(HKEY root_key, const String& key)
{
	HKEY h_key;
	DWORD disp = 0;
	LONG res = ::RegCreateKeyEx(
					root_key, TEXT(key.text()),
					0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS,
					NULL, &h_key, &disp);
	if (res == ERROR_SUCCESS)
		::RegCloseKey(h_key);
		return true;
	return false;
}

bool Registry::SetValue(HKEY root_key, const String& key, const String& name, const String& value)
{
	HKEY h_key;
	LONG result = ::RegOpenKeyEx(root_key,
					key.text(),
					0,
					KEY_SET_VALUE,
					&h_key);
	if (result != ERROR_SUCCESS)
		return false;
	
	result = ::RegSetValueEx(h_key, name.text(), 0, REG_SZ, value.bytes(), value.length());
	::RegCloseKey(h_key);
	return result == ERROR_SUCCESS;
}

bool Registry::SetValue(HKEY root_key, const String& key, const String& name, const int32* value)
{
	HKEY h_key;
	LONG result = ::RegOpenKeyEx(root_key,
					key.text(),
					0,
					KEY_SET_VALUE,
					&h_key);
	if (result != ERROR_SUCCESS)
		return false;
	
	result == ::RegSetValueEx(h_key, name.text(), 0, REG_DWORD, (const byte*)value, sizeof(int32));
	::RegCloseKey(h_key);
	return result == ERROR_SUCCESS;
}
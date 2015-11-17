/*	Copyright (c) 2015 SGH
**	
**	Permission is granted to use, modify and redistribute this software.
**	Modified versions of this software MUST be marked as such.
**	
**	This software is provided "AS IS". In no event shall
**	the authors or copyright holders be liable for any claim,
**	damages or other liability. The above copyright notice
**	and this permission notice shall be included in all copies
**	or substantial portions of the software.
**	
**	File created on: 15/11/2015
*/

#include "reference.h"

namespace Lua {
	Reference::Reference(int refKey) : m_referenceKey(refKey) {}
	Reference::operator bool() const noexcept { return (m_referenceKey != LUA_REFNIL) && (m_referenceKey != LUA_NOREF); }
	int Reference::key() const noexcept { return m_referenceKey; }
}

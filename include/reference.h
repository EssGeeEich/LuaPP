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

#ifndef __LUAPP_REFERENCE_H__
#define __LUAPP_REFERENCE_H__
#include "luainclude.h"

namespace Lua {
	class Reference {
		friend class State;
		int m_referenceKey;
	protected:
		explicit Reference(int refKey);
	public:
		explicit operator bool() const noexcept;
		int key() const noexcept;
	};
}
#endif // __LUAPP_REFERENCE_H__

/*	Copyright (c) 2023 Mauro Grassia
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
*/

#ifndef LUAPP_REFERENCE_HPP
#define LUAPP_REFERENCE_HPP

#include "FwdDecl.hpp"
#include <memory>

namespace Lua {
	class Reference {
		friend class State;
		
	protected:
		Reference();
		explicit Reference(std::weak_ptr<Lua::State> state, int refTable, int refKey);
		
	public:
		~Reference();
		Reference(Reference&&);
		Reference& operator= (Reference&&);
		
		void destroy();
		explicit operator bool() const noexcept;
		std::weak_ptr<Lua::State> state() const noexcept;
		int key() const noexcept;
		int table() const noexcept;
		
	private:
		Reference(Reference const&) =delete;
		Reference& operator= (Reference const&) =delete;
		void reset();
		
		std::weak_ptr<Lua::State> m_state;
		int m_refTable;
		int m_refKey;
	};

	typedef std::shared_ptr<Reference> ReferenceType;
}
#endif

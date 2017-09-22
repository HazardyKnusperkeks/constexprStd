/**
 ** This file is part of the constexprStd project.
 ** Copyright 2017 Björn Schäpers <bjoern@hazardy.de>.
 **
 ** This program is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU Lesser General Public License as
 ** published by the Free Software Foundation, either version 3 of the
 ** License, or (at your option) any later version.
 **
 ** This program is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU Lesser General Public License for more details.
 **
 ** You should have received a copy of the GNU Lesser General Public License
 ** along with this program.  If not, see <http://www.gnu.org/licenses/>.
 **/

/**
 * @file
 * @brief Contains details for the constexpr variants of <set>
 */

#ifndef CONSTEXPRSTD_DETAILS_SET_HPP
#define CONSTEXPRSTD_DETAILS_SET_HPP

#include <cstddef>
#include <iterator>
#include <type_traits>
#include <utility>

#include "../utility"
#include "uninitialized.hpp"

namespace constexprStd {
template<typename, std::size_t, typename, template<typename, std::size_t> typename, typename, std::size_t>
class set_base;
} //namespace constexprStd

namespace constexprStd::details {
struct NodeTag {};

template<typename Key>
struct SetNode {
	using ContentType         = Key;
	using InternalContentType = Uninitialized<ContentType>;
	
	InternalContentType Content;
	SetNode *Parent     = nullptr;
	SetNode *LeftChild  = nullptr;
	SetNode *RightChild = nullptr;
	bool Red            = true;
	bool PastEnd        = true;
	
	constexpr SetNode(void) = default;
	
	template<typename... Args>
	constexpr SetNode(NodeTag, Args&&... args)
			noexcept(std::is_nothrow_constructible_v<Key, Args&&...>) :
			Content{std::forward<Args>(args)...}, PastEnd{false} {
		return;
	}
	
	constexpr SetNode& operator=(SetNode&& that)
			noexcept(std::conjunction_v<std::is_nothrow_move_assignable<InternalContentType>,
			                            std::is_nothrow_move_constructible<InternalContentType>,
			                            std::is_nothrow_destructible<InternalContentType>>) {
		const auto oldThatPastEnd = that.PastEnd;
		//The PastEnd flag also indicates wether InternalContent is initialized.
		if ( !that.PastEnd ) {
			//The element to move from is initialized...
			if ( !PastEnd ) {
				//... and so are we, just move.
				Content.get() = std::move(that.Content).get();
			} //if ( !PastEnd )
			else {
				//... but we aren't, we have to initialize.
				Content.init(std::move(that.Content).get());
			} //else -> if ( !PastEnd )
			//And finally deinit the moved from content.
			that.reset();
		} //if ( !that.PastEnd )
		else {
			//We move from a not initialized
			reset();
		} //else -> if ( !that.PastEnd )
		Parent     = constexprStd::exchange(that.Parent,     nullptr);
		LeftChild  = constexprStd::exchange(that.LeftChild,  nullptr);
		RightChild = constexprStd::exchange(that.RightChild, nullptr);
		Red        = constexprStd::exchange(that.Red,        true);
		PastEnd    = oldThatPastEnd;
		return *this;
	}
	
	constexpr void reset(void) noexcept {
		if ( !PastEnd ) {
			Content.deinit();
			PastEnd = true;
		} //if ( !PastEnd )
		return;
	}
	
	[[nodiscard]] constexpr const ContentType& getContent(void) const noexcept {
		return Content.get();
	}
	
	[[nodiscard]] constexpr bool hasChildren(void) const noexcept {
		return LeftChild || RightChild;
	}
	
	[[nodiscard]] constexpr SetNode* next(void) const noexcept {
		if ( LeftChild ) {
			return LeftChild;
		} //if ( LeftChild )
		if ( RightChild ) {
			return RightChild;
		} //if ( RightChild )
		
		SetNode *current = this;
		do { //while ( current )
			SetNode *parent = current->Parent;
			if ( parent && parent->RightChild != current ) {
				return parent->RightChild;
			} //if ( parent && parent->RightChild != current )
		} while ( current );
		return nullptr;
	}
	
	[[nodiscard]] constexpr SetNode* prev(void) const noexcept {
		if ( !Parent ) {
			return nullptr;
		} //if ( !Parent )
		
		if ( this == Parent->LeftChild ) {
			return Parent;
		} //if ( this == Parent->LeftChild )
		
		//We are the right child
		if ( Parent->LeftChild == nullptr ) {
			//But we are an only child
			return Parent;
		} //if ( Parent->LeftChild == nullptr )
		
		SetNode *current = Parent->LeftChild;
		while ( current->hasChildren() ) {
			if ( current->LeftChild ) {
				current = current->LeftChild;
			} //if ( current->LeftChild )
			else {
				current = current->RightChild;
			} //else -> if ( current->LeftChild )
		} //while ( current->hasChildren() )
		return current;
	}
	
	constexpr void markForAdoption(void) noexcept {
		if ( Parent ) {
			if ( Parent->LeftChild == this ) {
				Parent->LeftChild = nullptr;
			} //if ( Parent->LeftChild == this )
			else {
				Parent->RightChild = nullptr;
			} //else -> if ( Parent->LeftChild == this )
			Parent = nullptr;
		} //if ( Parent )
		return;
	}
	
	constexpr void adoptLeft(SetNode *child) noexcept {
		LeftChild     = child;
		child->Parent = this;
		return;
	}
	
	constexpr void adoptRight(SetNode *child) noexcept {
		RightChild    = child;
		child->Parent = this;
		return;
	}
};

template<typename Key>
class SetNodeIterator {
	private:
	using NodeType = SetNode<Key>;
	
	public:
	using value_type        = typename NodeType::ContentType;
	using reference         = const value_type&;
	using pointer           = const value_type*;
	using difference_type   = std::ptrdiff_t;
	using iterator_category = std::bidirectional_iterator_tag;
	
	private:
	NodeType *Node = nullptr;
	
	constexpr SetNodeIterator(NodeType *node) noexcept : Node{node} {
		return;
	}
	
	public:
	constexpr SetNodeIterator(void) noexcept = default;
	
	[[nodiscard]] constexpr reference operator*(void) const noexcept {
		return Node->getContent();
	}
	
	[[nodiscard]] constexpr pointer operator->(void) const noexcept {
		return std::addressof(Node->getContent());
	}
	
	constexpr SetNodeIterator& operator++(void) const noexcept {
		Node = Node->next();
		return *this;
	}
	
	[[nodiscard]] constexpr SetNodeIterator operator++(int) const noexcept {
		SetNodeIterator copy(*this);
		++*this;
		return copy;
	}
	
	constexpr SetNodeIterator& operator--(void) const noexcept {
		Node = Node->prev();
		return *this;
	}
	
	[[nodiscard]] constexpr SetNodeIterator operator--(int) const noexcept {
		SetNodeIterator copy(*this);
		--*this;
		return copy;
	}
	
	[[nodiscard]] constexpr bool operator==(const SetNodeIterator that) const noexcept {
		if ( Node == that.Node ) {
			return true;
		} //if ( Node == that.Node )
		
		//From here on, only one is a default constructed iterator
		if ( Node == nullptr ) {
			//We are it, so we are equal iff that is past the end, a.k.a. the content pointer of the node is nullptr
			return that.Node->PastEnd;
		} //if ( Node == nullptr )
		
		//The other one is default
		return Node->PastEnd;
	}
	
	[[nodiscard]] constexpr bool operator!=(const SetNodeIterator that) const noexcept {
		return !(*this == that);
	}
	
	template<typename K, typename V, bool IsConst2>
	friend class NodeIterator;
	
	template<typename, std::size_t, typename, template<typename, std::size_t> typename, typename, std::size_t>
	friend class constexprStd::set_base;
};

} //namespace constexprStd::details

#endif

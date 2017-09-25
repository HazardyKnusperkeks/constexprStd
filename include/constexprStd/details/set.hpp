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

enum class NodeColor {
	Red,
	Black
};

enum class NodeDirection {
	None,
	Left,
	Right
};

template<typename Key>
struct SetNode {
	using ContentType         = Key;
	using InternalContentType = Uninitialized<ContentType>;
	
	InternalContentType Content;
	SetNode *Parent     = nullptr;
	SetNode *LeftChild  = nullptr;
	SetNode *RightChild = nullptr;
	NodeColor Color     = NodeColor::Red;
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
		Color      = constexprStd::exchange(that.Color,      NodeColor::Red);
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
		return hasLeftChild() || hasRightChild();
	}
	
	[[nodiscard]] constexpr bool hasLeftChild(void) const noexcept {
		return LeftChild;
	}
	
	[[nodiscard]] constexpr bool isLeftChild(const SetNode *node) const noexcept {
		return LeftChild == node;
	}
	
	[[nodiscard]] constexpr bool hasRightChild(void) const noexcept {
		return RightChild;
	}
	
	[[nodiscard]] constexpr bool isRightChild(const SetNode *node) const noexcept {
		return RightChild == node;
	}
	
	[[nodiscard]] constexpr bool hasParent(void) const noexcept {
		return Parent;
	}
	
	[[nodiscard]] constexpr SetNode* otherChild(const SetNode *node) const noexcept {
		if ( isLeftChild(node) ) {
			return RightChild;
		} //if ( isLeftChild(node) )
		return LeftChild;
	}
	
	[[nodiscard]] constexpr SetNode* leftestNode(void) noexcept {
		auto node = this;
		while ( node->hasLeftChild() ) {
			node = node->LeftChild;
		} //while ( node->hasLeftChild() )
		return node;
	}
	
	[[nodiscard]] constexpr const SetNode* leftestNode(void) const noexcept {
		auto node = this;
		while ( node->hasLeftChild() ) {
			node = node->LeftChild;
		} //while ( node->hasLeftChild() )
		return node;
	}
	
	[[nodiscard]] constexpr SetNode* rightestNode(void) noexcept {
		auto node = this;
		while ( node->hasRightChild() ) {
			node = node->RightChild;
		} //while ( node->hasRightChild() )
		return node;
	}
	
	[[nodiscard]] constexpr const SetNode* rightestNode(void) const noexcept {
		auto node = this;
		while ( node->hasRightChild() ) {
			node = node->RightChild;
		} //while ( node->hasRightChild() )
		return node;
	}
	
	[[nodiscard]] constexpr SetNode* fancyThis(void) const noexcept {
		if ( Parent->isLeftChild(this) ) {
			return Parent->LeftChild;
		} //if ( Parent->isLeftChild(this) )
		return Parent->RightChild;
	}
	
	[[nodiscard]] constexpr SetNode* next(void) const noexcept {
		if ( hasRightChild() ) {
			return RightChild->leftestNode();
		} //if ( hasRightChild() )
		
		if ( !hasParent() ) {
			//We are the root, the next one is always on the right side, but it is empty
			return nullptr;
		} //if ( !hasParent() )
		
		for ( SetNode *node = fancyThis(); node && node->hasParent(); node = node->Parent ) {
			if ( node->Parent->isLeftChild(node) ) {
				return node->Parent;
			} //if ( node->Parent->isLeftChild(node) )
		} //for ( SetNode *node = fancyThis(); node && node->hasParent(); node = node->Parent )
		return nullptr;
	}
	
#pragma GCC diagnostic push
//The nullpointer derefence may only happen if you decrement a begin iterator, which is undefined behaviour
#pragma GCC diagnostic ignored "-Wnull-dereference"
	[[nodiscard]] constexpr SetNode* prev(void) const noexcept {
		if ( hasLeftChild() ) {
			return LeftChild->rightestNode();
		} //if ( hasLeftChild() )
		
		if ( !hasParent() ) {
			//We are the root, the next one is always on the left side, but it is empty
			return nullptr;
		} //if ( !hasParent() )
		
		for ( SetNode *node = fancyThis(); node && node->hasParent(); node = node->Parent ) {
			if ( node->Parent->isRightChild(node) ) {
				return node->Parent;
			} //if ( node->Parent->isRightChild(node) )
		} //for ( SetNode *node = fancyThis(); node && node->hasParent(); node = node->Parent )
		return nullptr;
	}
#pragma GCC diagnostic pop
	
	constexpr NodeDirection markForAdoption(void) noexcept {
		NodeDirection ret = NodeDirection::None;
		if ( Parent ) {
			if ( Parent->isRightChild(this) ) {
				Parent->RightChild = nullptr;
				ret = NodeDirection::Right;
			} //if ( Parent->isRightChild(this) )
			else {
				Parent->LeftChild = nullptr;
				ret = NodeDirection::Left;
			} //else -> if ( Parent->isRightChild(this) )
			Parent = nullptr;
		} //if ( Parent )
		return ret;
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
	
	constexpr void adopt(SetNode *child, const NodeDirection dir) noexcept {
		switch ( dir ) {
			case NodeDirection::None  : std::terminate();
			case NodeDirection::Left  : adoptLeft(child); break;
			case NodeDirection::Right : adoptRight(child); break;
		} //switch ( dir )
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
	const NodeType *Node = nullptr;
	
	constexpr SetNodeIterator(const NodeType *node) noexcept : Node{node} {
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
	
	constexpr SetNodeIterator& operator++(void) noexcept {
		Node = Node->next();
		return *this;
	}
	
	[[nodiscard]] constexpr SetNodeIterator operator++(int) const noexcept {
		SetNodeIterator copy(*this);
		++*this;
		return copy;
	}
	
	constexpr SetNodeIterator& operator--(void) noexcept {
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
			//This takes care of really equal iterators and two default constructed ones
			return true;
		} //if ( Node == that.Node )
		
		if ( Node != nullptr && that.Node != nullptr ) {
			//Two non default constructed iterators with different nodes
			return false;
		} //if ( Node != nullptr && that.Node != nullptr )
		
		//From here on exactly one is a default constructed iterator
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

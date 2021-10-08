#pragma once

#include <concepts>
#include <utility>
#include <vector>

namespace LCross::Common::AST {
	template <class T>
	concept CMConstructible = std::copy_constructible<T>&& std::move_constructible<T>;

	template <CMConstructible TokenType, CMConstructible NodeType>
	struct ASTNode {
	public:
		using Type = ASTNode<TokenType, NodeType>;

	public:
		ASTNode(const Type& copy) : m_Token(copy.m_Token), m_Type(copy.m_Type), m_Children(copy.m_Children) { }
		ASTNode(Type&& move) : m_Token(std::move(move.m_Token)), m_Type(std::move(move.m_Type)), m_Children(std::move(move.m_Children)) { }

		ASTNode(const TokenType& token, const NodeType& type) : m_Token(token), m_Type(type) { }
		ASTNode(TokenType&& token, NodeType&& type) : m_Token(std::move(token)), m_Type(std::move(type)) { }

		auto& getToken() { return m_Token; }
		auto& getToken() const { return m_Token; }
		auto& getType() { return m_Type; }
		auto& getType() const { return m_Type; }
		auto& getChildren() { return m_Children; }
		auto& getChildren() const { return m_Children; }

		void pushChild(const ASTNode& node) { m_Children.push_back(node); }
		void pushChild(ASTNode&& node) { m_Children.push_back(std::move(node)); }
		void emplaceChild(TokenType&& token, NodeType&& type) {
			m_Children.emplace_back(std::move(token), std::move(type));
		}

	private:
		TokenType m_Token;
		NodeType m_Type;
		std::vector<Type> m_Children;
	};

	template <CMConstructible TokenType, CMConstructible NodeType>
	struct AST {
	public:
		using ASTType = ASTNode<TokenType, NodeType>;

	public:
		AST(const AST& copy) : m_RootNode(copy.m_RootNode) { }
		AST(AST&& move) : m_RootNode(std::move(move.m_RootNode)) { }

		AST(const ASTType& rootNode) : m_RootNode(rootNode) { }
		AST(ASTType&& rootNode) : m_RootNode(std::move(rootNode)) { }

		AST(const TokenType& token, const NodeType& type) : m_RootNode(token, type) { }
		AST(TokenType&& token, NodeType&& type) : m_RootNode(std::move(token), std::move(type)) { }

		auto& getRootNode() { return m_RootNode; }
		auto& getRootNode() const { return m_RootNode; }

	private:
		ASTType m_RootNode;
	};
} // namespace LCross::Common::AST
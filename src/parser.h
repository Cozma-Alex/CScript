#pragma once

#include <variant>

#include "tokenization.h"

struct NodeExpressionInteger {
    Token integer;
};

struct NodeExpressionIdentifier {
    Token identifier;
};

struct NodeExpression {
    std::variant<NodeExpressionInteger, NodeExpressionIdentifier> var;
};

struct NodeExitStatement {
    NodeExpression expression;
};

struct NodeVarStatement {
    Token identifier;
    NodeExpression expression;
};

struct NodeStatement {
    std::variant<NodeExitStatement, NodeVarStatement> var;
};

struct NodeProgram {
    std::vector<NodeStatement> statements;
};

class Parser {
public:

    inline explicit Parser(std::vector<Token> tokens) : m_tokens(std::move(tokens)) { m_index = 0; }


    inline std::optional<NodeExpression> parse_expr() {
        if (peek().has_value() and peek().value().type == TokenType::integer) {
            return NodeExpression{.var  = NodeExpressionInteger{.integer = consume()}};
        } else if (peek().has_value() and peek().value().type == TokenType::identifier) {
            return NodeExpression{.var = NodeExpressionIdentifier{.identifier = consume()}};
        } else {
            return {};
        }
    }

    std::optional<NodeStatement> parse_statement() {
        if (peek().has_value() and peek().value().type == TokenType::exit and peek(1).has_value()
            and peek(1).value().type == TokenType::open_parenthesis) {
            consume();
            consume();
            NodeExitStatement stmt_exit;
            if (auto node_expr = parse_expr()) {
                stmt_exit = {.expression = node_expr.value()};
            } else {
                std::cerr << "Invalid expression" << std::endl;
                exit(EXIT_FAILURE);
            }
            if (peek().has_value() and peek().value().type == TokenType::close_parenthesis) {
                consume();
            } else {
                std::cerr << "Expected `)`" << std::endl;
                exit(EXIT_FAILURE);
            }
            if (peek().has_value() and peek().value().type == TokenType::semi_column) {
                consume();
            } else {
                std::cerr << "Expected `;`" << std::endl;
                exit(EXIT_FAILURE);
            }
            return NodeStatement{.var = stmt_exit};
        } else if (peek().has_value() and peek().value().type == TokenType::var
                   and peek(1).has_value() and peek(1).value().type == TokenType::identifier
                   and peek(2).has_value() and peek(2).value().type == TokenType::equal_sign) {
            consume();
            auto statement_var = NodeVarStatement{.identifier = consume()};
            consume();
            if (auto expression = parse_expr()) {
                statement_var.expression = expression.value();
            } else {
                std::cerr << "Invalid expression" << std::endl;
                exit(EXIT_FAILURE);
            }
            if (peek().has_value() and peek().value().type == TokenType::semi_column) {
                consume();
            } else {
                std::cerr << "Expected `;`" << std::endl;
                exit(EXIT_FAILURE);
            }

            return NodeStatement{.var = statement_var};
        } else {
            return {};
        }
    }

    std::optional<NodeProgram> parse_program() {
        NodeProgram program;
        while (peek().has_value()) {
            if (auto statement = parse_statement()) {
                program.statements.push_back(statement.value());
            } else {
                std::cerr << "Invalid statement" << std::endl;
                exit(EXIT_FAILURE);
            }
        }
        return program;
    }

private:
    const std::vector<Token> m_tokens;

    size_t m_index;

    [[nodiscard]] inline std::optional<Token> peek(int offset = 0) const {
        if (m_index + offset >= m_tokens.size()) {
            return {};
        } else {
            return m_tokens.at(m_index + offset);
        }
    }

    inline Token consume() {
        return m_tokens.at(m_index++);
    }

};
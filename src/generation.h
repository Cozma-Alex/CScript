#pragma once

#include <unordered_map>
#include "parser.h"

class Generator{
public:

    inline explicit Generator(NodeProgram program): m_program(std::move(program)){}

    void generate_expression(const NodeExpression& expression){

        struct ExpressionVisitor{
            Generator* generator;

            void operator()(const NodeExpressionInteger& integer_expression) const{
                generator->m_output << "   mov rax, "<< integer_expression.integer.value.value()<<"\n";
                generator->push("rax");
            }

            void operator()(const NodeExpressionIdentifier& identifier_expression) const{
                if (generator->m_variables.find(identifier_expression.identifier.value.value()) == generator->m_variables.end())
                {
                    std::cerr<<"Undeclared identifier: "<<identifier_expression.identifier.value.value()<<std::endl;
                    exit(EXIT_FAILURE);
                }

                const auto& var = generator->m_variables.at(identifier_expression.identifier.value.value());
                std::stringstream offset;
                offset<<"QWORD [rsp + "<<(generator->m_stack_size - var.stack_location-1)*8<<"]";
                generator->push(offset.str());
            }
        };

        ExpressionVisitor visitor{.generator = this};
        std::visit(visitor,expression.var);
    }

    void generate_statement(const NodeStatement &statement){

        struct StatementVisitor{
            Generator* generator;
            void operator()(const NodeExitStatement& exitStatement) const{
                generator->generate_expression(exitStatement.expression);
                generator->m_output << "   mov rax,60\n";
                generator->pop("rdi");
                generator->m_output << "   syscall\n";
            }

            void operator()(const NodeVarStatement& varStatement) const{
                if(generator->m_variables.contains(varStatement.identifier.value.value())){
                    std::cerr<<"Identifier already used"<<varStatement.identifier.value.value()<<std::endl;
                    exit(EXIT_FAILURE);
                }

                generator->m_variables.insert({varStatement.identifier.value.value(),
                                               Variable{.stack_location = generator->m_stack_size}});

                generator->generate_expression(varStatement.expression);
            }
        };

        StatementVisitor visitor{.generator = this};
        std::visit(visitor,statement.var);

    }

    [[nodiscard]] std::string generate_program(){

        m_output << "   global _start\n   _start:\n";

        for (const NodeStatement& statement : m_program.statements) {
            generate_statement(statement);
        }

        m_output << "   mov rax,60\n";
        m_output << "   mov rdi,0\n";
        m_output << "   syscall\n";
        return m_output.str();
    }


private:

    void push(const std::string& reg){
        m_output << "   push "<< reg << "\n";
        m_stack_size++;
    }

    void pop(const std::string& reg){
        m_output<<"   pop "<<reg<<"\n";
        m_stack_size--;
    }

    struct Variable{
        size_t stack_location;
    };

    const NodeProgram m_program;
    std::stringstream m_output;
    size_t m_stack_size = 0;
    std::unordered_map<std::string, Variable> m_variables {};


};
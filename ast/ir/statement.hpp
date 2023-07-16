// author ~ cyclops

#pragma once

#include <type_traits>
#include <unordered_set>
#include <stdexcept>
#include <map>
#include <variant>
#include <unordered_map>

#include "node.hpp"
#include <iostream>

namespace deobf::ast::ir {
    namespace expression { // forward declare, to avoid circular deps
        struct expression;
        struct string_literal;
        struct name_and_args;
        struct variable;

        using expression_list_t = std::vector<std::shared_ptr<expression>>;
        using name_and_args_t = std::vector<std::shared_ptr<name_and_args>>;
        using variable_list_t = std::vector<std::shared_ptr<expression>>;  // variables are expressions aswell, but for more context make variable_list_t
        using name_list_t = std::vector<std::shared_ptr<string_literal>>;
    }

    using expression_t = typename expression::expression;


    /*
        stat
        : ';'
        | varlist '=' explist
        | functioncall (in expression for a reason)
        | 'break'
        | 'do' block 'end'
        | 'while' exp 'do' block 'end'
        | 'repeat' block 'until' exp
        | 'if' exp 'then' block ('elseif' exp 'then' block)* ('else' block)? 'end'
        | 'for' NAME '=' exp ',' exp (',' exp)? 'do' block 'end'
        | 'for' namelist 'in' explist 'do' block 'end'
        | 'function' funcname funcbody (in expression for a reason)
        | 'local' 'function' NAME funcbody
        | 'local' attnamelist ('=' explist)?
    */

    /*
        while
        return
        repeat
        if
        forstep
        forin
        assign
        local
    */


    namespace statement {
        struct statement : public node {
            virtual ~statement() = default;
        };

        using managed_statement_list = std::vector<std::shared_ptr<statement>>;

        struct return_statement final : public statement {
            ir::expression::expression_list_t body{ };

            std::string to_string() const override; // wanted to use std::copy on stringstream but lazy to define a custom type for node

            [[nodiscard]] bool equals(const node* other_node) const override;

            std::vector<std::shared_ptr<node>> get_children() const override;

            explicit return_statement(ir::expression::expression_list_t body) :
                body(std::move(body))
            { }

            void accept(abstract_visitor_pattern* visitor) override;
        };

        static const std::unordered_set<std::string> known_symbol_names{
            "string.byte",
            "string.char",
            "string.sub",
            "table.concat",
            "math.ldexp",
            "table.insert",
            "getfenv",
            "select",
            "unpack",
            "table.unpack",
            "tonumber",
            "setmetatable",
            "bit",
            "bit.bxor",
            "pcall",
        };

        // todo weak_symbol_info struct?

        struct symbol_info final { // no need a symbol table for each class, function/loops composite a block.
            enum class symbol_kind {
                global,
                local,
            };

            std::string symbol_name; // todo string_view since symbol name is seperate?

            std::string resolve_identifier; // symbol identifier for renaming purposes

            std::shared_ptr<statement> symbol_value;

            explicit symbol_info(std::string symbol_name) :
                symbol_name(symbol_name)
            { };

            explicit symbol_info(std::string symbol_name, std::shared_ptr<statement> symbol_value) :
                symbol_name(symbol_name),
                symbol_value(std::move(symbol_value))
            { };

            symbol_info& operator=(symbol_info& other) { // copy constructor (may confused here smth? idk)
                symbol_name = other.symbol_name;
                symbol_value = std::move(other.symbol_value);
                return *this;
            };

            [[nodiscard]] bool is_known_symbol() const noexcept;
        };
    }
}
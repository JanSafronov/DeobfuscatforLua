#pragma once

#include <variant>
#include <unordered_map>
#include <stdexcept>
#include <iomanip>
#include <iostream>
#include <sstream>

#include "statement.hpp"
#include "node.hpp"

/*
exp
    : 'nil' | 'false' | 'true'
    | number
    | string
    | '...' (parlist useless)
    | functiondef
    | prefixexp
    | tableconstructor
    | <assoc=right> exp operatorPower exp
    | operatorUnary exp
    | exp operatorMulDivMod exp
    | exp operatorAddSub exp
    | <assoc=right> exp operatorStrcat exp
    | exp operatorComparison exp
    | exp operatorAnd exp
    | exp operatorOr exp
    | exp operatorBitwise exp
*/

namespace deobf::ast::ir::expression {
    using statement_t = typename statement::statement;
    
    struct expression : public statement_t {
        virtual ~expression() = default;
    };

    // on visitor pattern method, we enumerate each element of the vector and visit it.
    using expression_list_t = std::vector<std::shared_ptr<expression>>;

    struct name_and_args final : public expression { // name:?(body)
        std::string name;
        expression_list_t body;

        std::string to_string() const override;

        [[nodiscard]] bool equals(const node* other_node) const override;

        std::vector<std::shared_ptr<node>> get_children() const override;

        explicit name_and_args(const std::string& name, expression_list_t body) :
            name(name),
            body(std::move(body))
        { }

        void accept(abstract_visitor_pattern* visitor) override;
    };

    using name_and_args_t = std::vector<std::shared_ptr<name_and_args>>;

    struct variable_suffix final : public expression {
        /*
        varSuffix
            : nameAndArgs* ('[' exp ']' | '.' NAME)
            ;
        */
        name_and_args_t name_and_args{ };

        std::string to_string() const override;

        [[nodiscard]] bool equals(const node* other_node) const override;

        std::vector<std::shared_ptr<node>> get_children() const override;

        std::shared_ptr<expression> name; // um

        explicit variable_suffix(std::shared_ptr<expression> name, name_and_args_t name_and_args) :
            name(std::move(name)),
            name_and_args(std::move(name_and_args))
        { }

        void accept(abstract_visitor_pattern* visitor) override;
    };

    struct variable final : public expression {
        std::shared_ptr<expression> name;
        std::vector<std::shared_ptr<variable_suffix>> suffixes;

        std::string to_string() const override;

        [[nodiscard]] bool equals(const node* other_node) const override;

        std::vector<std::shared_ptr<node>> get_children() const override;

        explicit variable(std::shared_ptr<expression> name, std::vector<std::shared_ptr<variable_suffix>> suffixes) :
            name(std::move(name)),
            suffixes(std::move(suffixes))
        { }

        void accept(abstract_visitor_pattern* visitor) override;
    };

    using variable_list_t = std::vector<std::shared_ptr<expression>>;  // variables are expressions aswell, but for more context make variable_list_t

    struct literal_base : public expression { // null object.
        std::string to_string() const override = 0;

        [[nodiscard]] bool equals(const node* other_node) const override = 0;

        void accept(abstract_visitor_pattern* visitor) override = 0;
    };

    struct nil_literal final : public literal_base {
        std::string to_string() const final override {
            return "nil";
        }

        [[nodiscard]] bool equals(const node* other_node) const final override {
            return dynamic_cast<const nil_literal*>(other_node) != nullptr;
        }

        void accept(abstract_visitor_pattern* visitor) final override;
    };

    struct numeral_literal final : public literal_base {
        double value; // lua numbers are double (IEEE754, info for decoding: sign=most signifcant bit, mantissa=52bits)

        std::string to_string() const final override {
            std::stringstream stream;

            stream << value; // todo deal with FPEs?

            return stream.str(); // better than retarded to_string
        }

        [[nodiscard]] bool equals(const node* other_node) const final override;

        explicit numeral_literal(const std::string& value) : value(std::stod(value)) { };
        explicit numeral_literal(double value) : value(value) { };

        void accept(abstract_visitor_pattern* visitor) final override;
    };

    struct array_index_literal final : public literal_base { // used to indicate array part indices (go till MAX_UINT)
        std::size_t value;

        std::string to_string() const final override {
            std::stringstream stream;

            stream << value; // todo deal with FPEs?

            return stream.str(); // better than retarded to_string
        }

        [[nodiscard]] bool equals(const node* other_node) const final override {
            return false;
        }

        explicit array_index_literal(std::size_t value) : value(value) { };

        void accept(abstract_visitor_pattern* visitor) final override;
    };

    struct boolean_literal final : public literal_base {
        bool value;

        std::string to_string() const final override {
            return value ? "true" : "false";
        }

        [[nodiscard]] bool equals(const node* other_node) const final override;

        explicit boolean_literal(bool value) : value(value) { };

        void accept(abstract_visitor_pattern* visitor) final override;
    };

    struct string_literal final : public literal_base {
        std::string value;

        std::string to_string() const final override {
            return value;
        }

        [[nodiscard]] bool equals(const node* other_node) const final override;

        explicit string_literal(const std::string& value) : value(value) { }

        void accept(abstract_visitor_pattern* visitor) final override;
    };
}
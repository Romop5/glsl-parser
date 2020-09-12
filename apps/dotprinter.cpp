#include <stdio.h>  // fread, fclose, fprintf, stderr
#include <string.h> // strcmp, memcpy

#include <string>
#include "glslParser/parser.hpp"

using namespace glsl;

#undef KEYWORD
#define KEYWORD(X) #X,
static const char *kTypes[] = {
    #include "glslParser/lexemes.hpp"
};
#undef KEYWORD
#define KEYWORD(...)

#undef OPERATOR
#define OPERATOR(N, S, P) S,
static const char *kOperators[] = {
    #include "glslParser/lexemes.hpp"
};
#undef OPERATOR
#define OPERATOR(...)

#define print(...) \
    do { \
        printTerminal(dotId, __VA_ARGS__); \
    } while (0)

class TUDOTprinter
{
    size_t currentID = 1;
    size_t getID()
    {
        return currentID++;
    }

    void printNode(size_t parentID, size_t ourID, const std::string nodeName, const std::string value = "")
    {
        printf("%d [label=\"%s\"]\n", ourID, nodeName.c_str());
        printf("%d -> %d\n", parentID, ourID);
    }

    void printTerminal(size_t parentID, const char* fmt, ...)
    {
        char buffer[2048];
        va_list args;
        va_start(args, fmt);
        vsprintf(buffer, fmt,args);
        va_end(args);

        static size_t ourID = 0;
        ourID++;
        printf("\"%d_%d\" [label=\"%s\" fillcolor=red style=filled]\n", parentID, ourID, buffer);
        printf("%d -> \"%d_%d\"\n", parentID, parentID, ourID);
    }

    private:
    void printBuiltin(size_t dotParent,astBuiltin *builtin) {
        const auto dotId = getID();
        const auto dotName = "astBuiltin";
        printNode(dotParent,dotId,dotName);
        print("%s", kTypes[builtin->type]);
    }

    void printType(size_t dotParent,astType *type) {
        const auto dotId = getID();
        const auto dotName = "astType";
        printNode(dotParent,dotId,dotName);
        if (type->builtin)
            printBuiltin(dotId,(astBuiltin*)type);
        else
            print("%s", ((astStruct*)type)->name);
    }

    void printIntConstant(size_t dotParent,astIntConstant *expression) {
        const auto dotId = getID();
        const auto dotName = "astIntConstant";
        printNode(dotParent,dotId,dotName);
        print("%d", expression->value);
    }

    void printUIntConstant(size_t dotParent,astUIntConstant *expression) {
        const auto dotId = getID();
        const auto dotName = "astUIntConstant";
        printNode(dotParent,dotId,dotName);
        print("%du", expression->value);
    }

    void printFloatConstant(size_t dotParent,astFloatConstant *expression) {
        const auto dotId = getID();
        const auto dotName = "astFloatConstant";
        printNode(dotParent,dotId,dotName);
        char format[1024];
        snprintf(format, sizeof format, "%g", expression->value);
        if (!strchr(format, '.'))
            print("%g.0", expression->value);
        else
            print("%s", format);
    }

    void printDoubleConstant(size_t dotParent,astDoubleConstant *expression) {
        const auto dotId = getID();
        const auto dotName = "astDoubleConstant";
        printNode(dotParent,dotId,dotName);
        print("%g", expression->value);
    }

    void printBoolConstant(size_t dotParent,astBoolConstant *expression) {
        const auto dotId = getID();
        const auto dotName = "astBoolConstant";
        printNode(dotParent,dotId,dotName);
        print("%s", expression->value ? "true" : "false");
    }

    void printVariable(size_t dotParent,astVariable *variable, bool nameOnly = false) {
        const auto dotId = getID();
        const auto dotName = "astVariable";
        printNode(dotParent,dotId,dotName);
        if (variable->isPrecise)
            print("precise ");

        //if (nameOnly) {
        //    print("%s", variable->name);
        //    return;
        //}

        printType(dotId,variable->baseType);
        print(" %s", variable->name);

        if (nameOnly)
            return;
    }


    void printGlobalVariable(size_t dotParent,astGlobalVariable *variable) {
        const auto dotId = getID();
        const auto dotName = "astGlobalVariable";
        printNode(dotParent,dotId,dotName);
        std::vector<astLayoutQualifier*> &qualifiers = variable->layoutQualifiers;
        if (variable->layoutQualifiers.size()) {
            print("layout (");
            for (size_t i = 0; i < qualifiers.size(); i++) {
                astLayoutQualifier *qualifier = qualifiers[i];
                print("%s", qualifier->name);
                if (qualifier->initialValue) {
                    print(" = ");
                    printExpression(dotId,qualifier->initialValue);
                }
                if (i != qualifiers.size() - 1)
                    print(", ");
            }
            print(") ");
        }

        if (variable->isInvariant)
            print("invariant ");

        switch (variable->interpolation) {
        case kSmooth:
            print("smooth ");
            break;
        case kFlat:
            print("flat ");
            break;
        case kNoPerspective:
            print("noperspective ");
            break;
        }

        printVariable(dotId,(astVariable*)variable);

        if (variable->initialValue) {
            print(" = ");
            printExpression(dotId,variable->initialValue);
        }

        print(";\n");
    }

    void printVariableIdentifier(size_t dotParent,astVariableIdentifier *expression) {
        const auto dotId = getID();
        const auto dotName = "astVariableIdentifier";
        printNode(dotParent,dotId,dotName);
        printVariable(dotId,expression->variable, true);
    }

    void printFieldOrSwizzle(size_t dotParent,astFieldOrSwizzle *expression) {
        const auto dotId = getID();
        const auto dotName = "astFieldOrSwizzle";
        printNode(dotParent,dotId,dotName);
        printExpression(dotId,expression->operand);
        print(".%s", expression->name);
    }

    void printArraySubscript(size_t dotParent,astArraySubscript *expression) {
        const auto dotId = getID();
        const auto dotName = "astArraySubscript";
        printNode(dotParent,dotId,dotName);
        printExpression(dotId,expression->operand);
        print("[");
        printExpression(dotId,expression->index);
        print("]");
    }

    void printFunctionCall(size_t dotParent,astFunctionCall *expression) {
        const auto dotId = getID();
        const auto dotName = "astFunctionCall";
        printNode(dotParent,dotId,dotName);
        print("%s(", expression->name);
        for (size_t i = 0; i < expression->parameters.size(); i++) {
            printExpression(dotId,expression->parameters[i]);
            if (i != expression->parameters.size() - 1)
                print(", ");
        }
        print(")");
    }

    void printConstructorCall(size_t dotParent,astConstructorCall *expression) {
        const auto dotId = getID();
        const auto dotName = "astConstructorCall";
        printNode(dotParent,dotId,dotName);
        printType(dotId,expression->type);
        print("(");
        for (size_t i = 0; i < expression->parameters.size(); i++) {
            printExpression(dotId,expression->parameters[i]);
            if (i != expression->parameters.size() - 1)
                print(", ");
        }
        print(")");
    }

    enum { kSemicolon = 1 << 0, kNewLine = 1 << 1, kDefault = kSemicolon | kNewLine };

    void printFunctionVariable(size_t dotParent,astFunctionVariable *variable, int flags = kDefault ) {
        const auto dotId = getID();
        const auto dotName = "astFunctionVariable";
        printNode(dotParent,dotId,dotName);
        if (variable->isConst)
            print("const ");
        printVariable(dotId,(astVariable*)variable);
        if (variable->initialValue) {
            print(" = ");
            printExpression(dotId,variable->initialValue);
        }
        if (flags & kSemicolon) print(";");
        if (flags & kNewLine) print("\n");
    }

    void printPostIncrement(size_t dotParent,astPostIncrementExpression *expression) {
        const auto dotId = getID();
        const auto dotName = "astPostIncrementExpression";
        printNode(dotParent,dotId,dotName);
        printExpression(dotId,expression->operand);
        print("++");
    }

    void printPostDecrement(size_t dotParent,astPostDecrementExpression *expression) {
        const auto dotId = getID();
        const auto dotName = "astPostDecrementExpression";
        printNode(dotParent,dotId,dotName);
        printExpression(dotId,expression->operand);
        print("--");
    }

    void printUnaryMinus(size_t dotParent,astUnaryMinusExpression *expression) {
        const auto dotId = getID();
        const auto dotName = "astUnaryMinusExpression";
        printNode(dotParent,dotId,dotName);
        print("-");
        printExpression(dotId,expression->operand);
    }

    void printUnaryPlus(size_t dotParent,astUnaryPlusExpression *expression) {
        const auto dotId = getID();
        const auto dotName = "astUnaryPlusExpression";
        printNode(dotParent,dotId,dotName);
        print("+");
        printExpression(dotId,expression->operand);
    }

    void printUnaryBitNot(size_t dotParent,astUnaryBitNotExpression *expression) {
        const auto dotId = getID();
        const auto dotName = "astUnaryBitNotExpression";
        printNode(dotParent,dotId,dotName);
        print("~");
        printExpression(dotId,expression->operand);
    }

    void printUnaryLogicalNot(size_t dotParent,astUnaryLogicalNotExpression *expression) {
        const auto dotId = getID();
        const auto dotName = "astUnaryLogicalNotExpression";
        printNode(dotParent,dotId,dotName);
        print("!");
        printExpression(dotId,expression->operand);
    }

    void printPrefixIncrement(size_t dotParent,astPrefixIncrementExpression *expression) {
        const auto dotId = getID();
        const auto dotName = "astPrefixIncrementExpression";
        printNode(dotParent,dotId,dotName);
        print("++");
        printExpression(dotId,expression->operand);
    }

    void printPrefixDecrement(size_t dotParent,astPrefixDecrementExpression *expression) {
        const auto dotId = getID();
        const auto dotName = "astPrefixDecrementExpression";
        printNode(dotParent,dotId,dotName);
        print("--");
        printExpression(dotId,expression->operand);
    }

    void printAssign(size_t dotParent,astAssignmentExpression *expression) {
        const auto dotId = getID();
        const auto dotName = "astAssignmentExpression";
        printNode(dotParent,dotId,dotName);
        printExpression(dotId,expression->operand1);
        print(" %s ", kOperators[expression->assignment]);
        printExpression(dotId,expression->operand2);
    }

    void printSequence(size_t dotParent,astSequenceExpression *expression) {
        const auto dotId = getID();
        const auto dotName = "astSequenceExpression";
        printNode(dotParent,dotId,dotName);
        print("(");
        printExpression(dotId,expression->operand1);
        print(", ");
        printExpression(dotId,expression->operand2);
        print(")");
    }

    void printOperation(size_t dotParent,astOperationExpression *expression) {
        const auto dotId = getID();
        const auto dotName = "astOperationExpression";
        printNode(dotParent,dotId,dotName);
        print("(");
        printExpression(dotId,expression->operand1);
        print(" %s ", kOperators[expression->operation]);
        printExpression(dotId,expression->operand2);
        print(")");
    }

    void printTernary(size_t dotParent,astTernaryExpression *expression) {
        const auto dotId = getID();
        const auto dotName = "astTernaryExpression";
        printNode(dotParent,dotId,dotName);
        print("(");
        printExpression(dotId,expression->condition);
        print(" ? ");
        printExpression(dotId,expression->onTrue);
        print(" : ");
        printExpression(dotId,expression->onFalse);
        print(")");
    }

    void printExpression(size_t dotParent,astExpression *expression) {
        const auto dotId = getID();
        const auto dotName = "astExpression";
        printNode(dotParent,dotId,dotName);
        switch (expression->type) {
        case astExpression::kIntConstant:
            return printIntConstant(dotId,(astIntConstant*)expression);
        case astExpression::kUIntConstant:
            return printUIntConstant(dotId,(astUIntConstant*)expression);
        case astExpression::kFloatConstant:
            return printFloatConstant(dotId,(astFloatConstant*)expression);
        case astExpression::kDoubleConstant:
            return printDoubleConstant(dotId,(astDoubleConstant*)expression);
        case astExpression::kBoolConstant:
            return printBoolConstant(dotId,(astBoolConstant*)expression);
        case astExpression::kVariableIdentifier:
            return printVariableIdentifier(dotId,(astVariableIdentifier*)expression);
        case astExpression::kFieldOrSwizzle:
            return printFieldOrSwizzle(dotId,(astFieldOrSwizzle*)expression);
        case astExpression::kArraySubscript:
            return printArraySubscript(dotId,(astArraySubscript*)expression);
        case astExpression::kFunctionCall:
            return printFunctionCall(dotId,(astFunctionCall*)expression);
        case astExpression::kConstructorCall:
            return printConstructorCall(dotId,(astConstructorCall*)expression);
        case astExpression::kPostIncrement:
            return printPostIncrement(dotId,(astPostIncrementExpression*)expression);
        case astExpression::kPostDecrement:
            return printPostDecrement(dotId,(astPostDecrementExpression*)expression);
        case astExpression::kUnaryMinus:
            return printUnaryMinus(dotId,(astUnaryMinusExpression*)expression);
        case astExpression::kUnaryPlus:
            return printUnaryPlus(dotId,(astUnaryPlusExpression*)expression);
        case astExpression::kBitNot:
            return printUnaryBitNot(dotId,(astUnaryBitNotExpression*)expression);
        case astExpression::kLogicalNot:
            return printUnaryLogicalNot(dotId,(astUnaryLogicalNotExpression*)expression);
        case astExpression::kPrefixIncrement:
            return printPrefixIncrement(dotId,(astPrefixIncrementExpression*)expression);
        case astExpression::kPrefixDecrement:
            return printPrefixDecrement(dotId,(astPrefixDecrementExpression*)expression);
        case astExpression::kAssign:
            return printAssign(dotId,(astAssignmentExpression*)expression);
        case astExpression::kSequence:
            return printSequence(dotId,(astSequenceExpression*)expression);
        case astExpression::kOperation:
            return printOperation(dotId,(astOperationExpression*)expression);
        case astExpression::kTernary:
            return printTernary(dotId,(astTernaryExpression*)expression);
        }
    }

    void printCompoundStatement(size_t dotParent,astCompoundStatement *statement) {
        const auto dotId = getID();
        const auto dotName = "astCompoundStatement";
        printNode(dotParent,dotId,dotName);
        print(" {\n");
        for (size_t i = 0; i < statement->statements.size(); i++)
            printStatement(dotId,statement->statements[i]);
        print("}\n");
    }

    void printDeclarationStatement(size_t dotParent,astDeclarationStatement *statement, int flags = kDefault) {
        const auto dotId = getID();
        const auto dotName = "astDeclarationStatement";
        printNode(dotParent,dotId,dotName);
        for (size_t i = 0; i < statement->variables.size(); i++)
            printFunctionVariable(dotId,statement->variables[i], flags);
    }

    void printExpressionStatement(size_t dotParent,astExpressionStatement *statement, int flags = kDefault) {
        const auto dotId = getID();
        const auto dotName = "astExpressionStatement";
        printNode(dotParent,dotId,dotName);
        printExpression(dotId,statement->expression);
        if (flags & kSemicolon) print(";");
        if (flags & kNewLine) print("\n");
    }

    void printIfStetement(size_t dotParent,astIfStatement *statement) {
        const auto dotId = getID();
        const auto dotName = "astIfStatement";
        printNode(dotParent,dotId,dotName);
        print("if(");
        printExpression(dotId,statement->condition);
        print(")");
        printStatement(dotId,statement->thenStatement);
        if (statement->elseStatement) {
            print("else");
            if (statement->elseStatement->type == astStatement::kIf)
                print(" ");
            printStatement(dotId,statement->elseStatement);
        }
    }

    void printSwitchStatement(size_t dotParent,astSwitchStatement *statement) {
        const auto dotId = getID();
        const auto dotName = "astSwitchStatement";
        printNode(dotParent,dotId,dotName);
        print("switch(");
        printExpression(dotId,statement->expression);
        print(") {\n");
        for (size_t i = 0; i < statement->statements.size(); i++)
            printStatement(dotId,statement->statements[i]);
        print("}\n");
    }

    void printCaseLabelStatement(size_t dotParent,astCaseLabelStatement *statement) {
        const auto dotId = getID();
        const auto dotName = "astCaseLabelStatement";
        printNode(dotParent,dotId,dotName);
        if (statement->isDefault)
            print("default");
        else {
            print("case ");
            printExpression(dotId,statement->condition);
        }
        print(":\n");
    }

    void printWhileStatement(size_t dotParent,astWhileStatement *statement) {
        const auto dotId = getID();
        const auto dotName = "astWhileStatement";
        printNode(dotParent,dotId,dotName);
        print("while(");
        switch (statement->condition->type) {
        case astStatement::kDeclaration:
            printDeclarationStatement(dotId,(astDeclarationStatement*)statement->condition, false);
            break;
        case astStatement::kExpression:
            printExpressionStatement(dotId,(astExpressionStatement*)statement->condition, false);
            break;
        }
        print(")");
        printStatement(dotId,statement->body);
    }

    void printDoStatement(size_t dotParent,astDoStatement *statement) {
        const auto dotId = getID();
        const auto dotName = "astDoStatement";
        printNode(dotParent,dotId,dotName);
        print("do");
        // deal with non compound (i.e scope) in do loops, e.g: do function_call(); while(expr);
        if (statement->body->type != astStatement::kCompound)
            print(" ");
        printStatement(dotId,statement->body);
        print("while(");
        printExpression(dotId,statement->condition);
        print(");\n");
    }

    void printForStatement(size_t dotParent,astForStatement *statement) {
        const auto dotId = getID();
        const auto dotName = "astForStatement";
        printNode(dotParent,dotId,dotName);
        print("for(");
        if (statement->init) {
            switch (statement->init->type) {
            case astStatement::kDeclaration:
                printDeclarationStatement(dotId,(astDeclarationStatement*)statement->init, kSemicolon);
                break;
            case astStatement::kExpression:
                printExpressionStatement(dotId,(astExpressionStatement*)statement->init, kSemicolon);
                break;
            }
        } else {
            print(";");
        }
        if (statement->condition) {
            print(" ");
            printExpression(dotId,statement->condition);
        }
        print(";");
        if (statement->loop) {
            print(" ");
            printExpression(dotId,statement->loop);
        }
        print(")");
        printStatement(dotId,statement->body);
    }

    void printReturnStatement(size_t dotParent,astReturnStatement *statement) {
        const auto dotId = getID();
        const auto dotName = "astReturnStatement";
        printNode(dotParent,dotId,dotName);
        if (statement->expression) {
            print("return ");
            printExpression(dotId,statement->expression);
            print(";\n");
        } else {
            print("return;\n");
        }
    }

    void printStatement(size_t dotParent,astStatement *statement) {
        const auto dotId = getID();
        const auto dotName = "astStatement";
        printNode(dotParent,dotId,dotName);
        switch (statement->type) {
        case astStatement::kCompound:
            return printCompoundStatement(dotId,(astCompoundStatement*)statement);
        case astStatement::kEmpty:
            ;//return printEmptyStatement(dotId);
        case astStatement::kDeclaration:
            return printDeclarationStatement(dotId,(astDeclarationStatement*)statement);
        case astStatement::kExpression:
            return printExpressionStatement(dotId,(astExpressionStatement*)statement);
        case astStatement::kIf:
            return printIfStetement(dotId,(astIfStatement*)statement);
        case astStatement::kSwitch:
            return printSwitchStatement(dotId,(astSwitchStatement*)statement);
        case astStatement::kCaseLabel:
            return printCaseLabelStatement(dotId,(astCaseLabelStatement*)statement);
        case astStatement::kWhile:
            return printWhileStatement(dotId,(astWhileStatement*)statement);
        case astStatement::kDo:
            return printDoStatement(dotId,(astDoStatement*)statement);
        case astStatement::kFor:
            return printForStatement(dotId,(astForStatement*)statement);
        case astStatement::kContinue:
            ;//return printContinueStatement(dotId);
        case astStatement::kBreak:
            ;//return printBreakStatement(dotId);
        case astStatement::kReturn:
            return printReturnStatement(dotId,(astReturnStatement*)statement);
        case astStatement::kDiscard:
            ;//return printDiscardStatement(dotId);
        }
        print("\n");
    }

    void printFunctionParameter(size_t dotParent,astFunctionParameter *parameter) {
        const auto dotId = getID();
        const auto dotName = "astFunctionParameter";
        printNode(dotParent,dotId,dotName);
        printType(dotId,parameter->baseType);
        if (parameter->name)
            print(" %s", parameter->name);
        if (parameter->isArray)
            ;//printArraySize(dotId,parameter->arraySizes);
    }

    void printFunction(size_t dotParent,astFunction *function) {
        const auto dotId = getID();
        const auto dotName = "astFunction";
        printNode(dotParent,dotId,dotName);
        printType(dotId,function->returnType);
        print(" %s(", function->name);
        for (size_t i = 0; i < function->parameters.size(); i++)
            printFunctionParameter(dotId,function->parameters[i]);
        print(")");
        if (function->isPrototype) {
            print(";\n");
            return;
        }
        print(" {\n");
        for (size_t i = 0; i < function->statements.size(); i++)
            printStatement(dotId,function->statements[i]);
        print("}\n");
    }

    void printStructure(size_t dotParent,astStruct *structure) {
        const auto dotId = getID();
        const auto dotName = "astStruct";
        printNode(dotParent,dotId,dotName);
        print("struct ");
        if (structure->name)
            print("%s ", structure->name);
        print("{\n");
        for (size_t i = 0; i < structure->fields.size(); i++) {
            printVariable(dotId,structure->fields[i]);
            print(";\n");
        }
        print("};\n");
    }

    public:
    void printTU(size_t dotParent,astTU *tu) {
        const auto dotId = getID();
        const auto dotName = "astTU";
        printNode(dotParent,dotId,dotName);
        for (size_t i = 0; i < tu->structures.size(); i++)
            printStructure(dotId,tu->structures[i]);
        for (size_t i = 0; i < tu->globals.size(); i++)
            printGlobalVariable(dotId,tu->globals[i]);
        for (size_t i = 0; i < tu->functions.size(); i++)
            printFunction(dotId,tu->functions[i]);
    }

}; // class TUDOTprinter

struct sourceFile {
    const char *fileName;
    FILE *file;
    int shaderType;
};

int main(int argc, char **argv) {
    int shaderType = -1;
    std::vector<sourceFile> sources;
    while (argc > 1) {
        ++argv;
        --argc;
        if (argv[0][0] == '-' && argv[0][1]) {
            const char *what = argv[0] + 1;
            if (!strcmp(what, "c"))
                shaderType = astTU::kCompute;
            else if (!strcmp(what, "v"))
                shaderType = astTU::kVertex;
            else if (!strcmp(what, "tc"))
                shaderType = astTU::kTessControl;
            else if (!strcmp(what, "te"))
                shaderType = astTU::kTessEvaluation;
            else if (!strcmp(what, "g"))
                shaderType = astTU::kGeometry;
            else if (!strcmp(what, "f"))
                shaderType = astTU::kFragment;
            else {
                fprintf(stderr, "unknown option: `%s'\n", argv[0]);
                return 1;
            }
        } else {
            // Treat as fragment shader by default
            if (shaderType == -1)
                shaderType = astTU::kFragment;
            sourceFile source;
            if (!strcmp(argv[0], "-")) {
                source.fileName = "<stdin>";
                source.file = stdin;
                source.shaderType = shaderType;
                sources.push_back(source);
            } else {
                source.fileName = argv[0];
                if ((source.file = fopen(argv[0], "r"))) {
                    source.shaderType = shaderType;
                    sources.push_back(source);
                } else {
                    fprintf(stderr, "failed to read shader file: `%s' (ignoring)\n", argv[0]);
                }
            }
        }
    }

    for (size_t i = 0; i < sources.size(); i++) {
        std::vector<char> contents;
        // Read contents of file
        if (sources[i].file != stdin) {
            fseek(sources[i].file, 0, SEEK_END);
            contents.resize(ftell(sources[i].file));
            fseek(sources[i].file, 0, SEEK_SET);
            fread(&contents[0], 1, contents.size(), sources[i].file);
            fclose(sources[i].file);
        } else {
            char buffer[1024];
            int c;
            while ((c = fread(buffer, 1, sizeof(buffer), stdin))) {
                contents.reserve(contents.size() + c);
                contents.insert(contents.end(), buffer, buffer + c);
            }
        }
        contents.push_back('\0');
        parser p(&contents[0], sources[i].fileName);
        astTU *tu = p.parse(sources[i].shaderType);
        if (tu) {
            TUDOTprinter printer;
            printf("digraph AST {\n");
            printf("0 [label=shader]\n");
            printer.printTU(0,tu);
            printf("}\n");
        } else {
            fprintf(stderr, "%s\n", p.error());
        }
    }
    return 0;
}

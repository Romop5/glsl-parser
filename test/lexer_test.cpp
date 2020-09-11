#include "gtest/gtest.h"
#include "glslParser/lexer.hpp"

#include <vector>
#include <string>
#include <sstream>
#include <iostream>

#define TEST_PAIR_KEYWORD(name) {""#name, glsl::kKeyword_##name}

#define EXPECT_KEYWORD(kwType)  {\
    auto tok = lex.read();\
    EXPECT_EQ(tok.getType(), glsl::kType_keyword);\
    EXPECT_EQ(tok.getAsKeyword(), glsl::kKeyword_##kwType);\
}

#define EXPECT_IDENTIFIER(str)  {\
    auto tok = lex.read();\
    EXPECT_EQ(tok.getType(), glsl::kType_identifier);\
    EXPECT_EQ(tok.getAsIdentifier(), std::string(str));\
}

#define EXPECT_SCOPE_BEGIN()  {\
    auto tok = lex.read();\
    EXPECT_EQ(tok.getType(), glsl::kType_scope_begin);\
}

#define EXPECT_SCOPE_END()  {\
    auto tok = lex.read();\
    EXPECT_EQ(tok.getType(), glsl::kType_scope_end);\
}

#define EXPECT_PARENTHESIS_BEGIN()  {\
    auto tok = lex.read();\
    EXPECT_EQ(tok.getType(), glsl::kType_operator);\
    EXPECT_EQ(tok.getAsOperator(), glsl::kOperator_paranthesis_begin);\
}

#define EXPECT_PARENTHESIS_END()  {\
    auto tok = lex.read();\
    EXPECT_EQ(tok.getType(), glsl::kType_operator);\
    EXPECT_EQ(tok.getAsOperator(), glsl::kOperator_paranthesis_end);\
}

namespace {
// Try out all keywords according to
// https://www.khronos.org/registry/OpenGL/specs/gl/GLSLangSpec.4.40.pdf specification
TEST(Lexer, AllKeywords) {

    // https://www.khronos.org/registry/OpenGL/specs/gl/GLSLangSpec.4.40.pdf specification
    std::vector<std::pair<std::string,int>> keywords = {
        TEST_PAIR_KEYWORD(attribute),
        TEST_PAIR_KEYWORD(const),
        TEST_PAIR_KEYWORD(uniform),
        TEST_PAIR_KEYWORD(varying),
        TEST_PAIR_KEYWORD(buffer),
        TEST_PAIR_KEYWORD(shared),
        TEST_PAIR_KEYWORD(coherent),
        TEST_PAIR_KEYWORD(volatile),
        TEST_PAIR_KEYWORD(restrict),
        TEST_PAIR_KEYWORD(readonly),
        TEST_PAIR_KEYWORD(writeonly),
        TEST_PAIR_KEYWORD(atomic_uint),
        TEST_PAIR_KEYWORD(layout),
        TEST_PAIR_KEYWORD(centroid),
        TEST_PAIR_KEYWORD(flat),
        TEST_PAIR_KEYWORD(smooth),
        TEST_PAIR_KEYWORD(noperspective),
        TEST_PAIR_KEYWORD(patch),
        TEST_PAIR_KEYWORD(sample),
        TEST_PAIR_KEYWORD(break),
        TEST_PAIR_KEYWORD(continue),
        TEST_PAIR_KEYWORD(do),
        TEST_PAIR_KEYWORD(for),
        TEST_PAIR_KEYWORD(while),
        TEST_PAIR_KEYWORD(switch),
        TEST_PAIR_KEYWORD(case),
        TEST_PAIR_KEYWORD(default),
        TEST_PAIR_KEYWORD(if),
        TEST_PAIR_KEYWORD(else),
        TEST_PAIR_KEYWORD(subroutine),
        TEST_PAIR_KEYWORD(in),
        TEST_PAIR_KEYWORD(out),
        TEST_PAIR_KEYWORD(inout),
        TEST_PAIR_KEYWORD(float),
        TEST_PAIR_KEYWORD(double),
        TEST_PAIR_KEYWORD(int),
        TEST_PAIR_KEYWORD(void),
        TEST_PAIR_KEYWORD(bool),
        TEST_PAIR_KEYWORD(true),
        TEST_PAIR_KEYWORD(false),
        TEST_PAIR_KEYWORD(invariant),
        TEST_PAIR_KEYWORD(precise),
        TEST_PAIR_KEYWORD(discard),
        TEST_PAIR_KEYWORD(return),
        TEST_PAIR_KEYWORD(mat2),
        TEST_PAIR_KEYWORD(mat3),
        TEST_PAIR_KEYWORD(mat4),
        TEST_PAIR_KEYWORD(dmat2),
        TEST_PAIR_KEYWORD(dmat3),
        TEST_PAIR_KEYWORD(dmat4),
        TEST_PAIR_KEYWORD(mat2x2),
        TEST_PAIR_KEYWORD(mat2x3),
        TEST_PAIR_KEYWORD(mat2x4),
        TEST_PAIR_KEYWORD(dmat2x2),
        TEST_PAIR_KEYWORD(dmat2x3),
        TEST_PAIR_KEYWORD(dmat2x4),
        TEST_PAIR_KEYWORD(mat3x2),
        TEST_PAIR_KEYWORD(mat3x3),
        TEST_PAIR_KEYWORD(mat3x4),
        TEST_PAIR_KEYWORD(dmat3x2),
        TEST_PAIR_KEYWORD(dmat3x3),
        TEST_PAIR_KEYWORD(dmat3x4),
        TEST_PAIR_KEYWORD(mat4x2),
        TEST_PAIR_KEYWORD(mat4x3),
        TEST_PAIR_KEYWORD(mat4x4),
        TEST_PAIR_KEYWORD(dmat4x2),
        TEST_PAIR_KEYWORD(dmat4x3),
        TEST_PAIR_KEYWORD(dmat4x4),
        TEST_PAIR_KEYWORD(vec2),
        TEST_PAIR_KEYWORD(vec3),
        TEST_PAIR_KEYWORD(vec4),
        TEST_PAIR_KEYWORD(ivec2),
        TEST_PAIR_KEYWORD(ivec3),
        TEST_PAIR_KEYWORD(ivec4),
        TEST_PAIR_KEYWORD(bvec2),
        TEST_PAIR_KEYWORD(bvec3),
        TEST_PAIR_KEYWORD(bvec4),
        TEST_PAIR_KEYWORD(dvec2),
        TEST_PAIR_KEYWORD(dvec3),
        TEST_PAIR_KEYWORD(dvec4),
        TEST_PAIR_KEYWORD(uint),
        TEST_PAIR_KEYWORD(uvec2),
        TEST_PAIR_KEYWORD(uvec3),
        TEST_PAIR_KEYWORD(uvec4),
        TEST_PAIR_KEYWORD(lowp),
        TEST_PAIR_KEYWORD(mediump),
        TEST_PAIR_KEYWORD(highp),
        TEST_PAIR_KEYWORD(precision),
        TEST_PAIR_KEYWORD(sampler1D),
        TEST_PAIR_KEYWORD(sampler2D),
        TEST_PAIR_KEYWORD(sampler3D),
        TEST_PAIR_KEYWORD(samplerCube),
        TEST_PAIR_KEYWORD(sampler1DShadow),
        TEST_PAIR_KEYWORD(sampler2DShadow),
        TEST_PAIR_KEYWORD(samplerCubeShadow),
        TEST_PAIR_KEYWORD(sampler1DArray),
        TEST_PAIR_KEYWORD(sampler2DArray),
        TEST_PAIR_KEYWORD(sampler1DArrayShadow),
        TEST_PAIR_KEYWORD(sampler2DArrayShadow),
        TEST_PAIR_KEYWORD(isampler1D),
        TEST_PAIR_KEYWORD(isampler2D),
        TEST_PAIR_KEYWORD(isampler3D),
        TEST_PAIR_KEYWORD(isamplerCube),
        TEST_PAIR_KEYWORD(isampler1DArray),
        TEST_PAIR_KEYWORD(isampler2DArray),
        TEST_PAIR_KEYWORD(usampler1D),
        TEST_PAIR_KEYWORD(usampler2D),
        TEST_PAIR_KEYWORD(usampler3D),
        TEST_PAIR_KEYWORD(usamplerCube),
        TEST_PAIR_KEYWORD(usampler1DArray),
        TEST_PAIR_KEYWORD(usampler2DArray),
        TEST_PAIR_KEYWORD(sampler2DRect),
        TEST_PAIR_KEYWORD(sampler2DRectShadow),
        TEST_PAIR_KEYWORD(isampler2DRect),
        TEST_PAIR_KEYWORD(usampler2DRect),
        TEST_PAIR_KEYWORD(samplerBuffer),
        TEST_PAIR_KEYWORD(isamplerBuffer),
        TEST_PAIR_KEYWORD(usamplerBuffer),
        TEST_PAIR_KEYWORD(sampler2DMS),
        TEST_PAIR_KEYWORD(isampler2DMS),
        TEST_PAIR_KEYWORD(usampler2DMS),
        TEST_PAIR_KEYWORD(sampler2DMSArray),
        TEST_PAIR_KEYWORD(isampler2DMSArray),
        TEST_PAIR_KEYWORD(usampler2DMSArray),
        TEST_PAIR_KEYWORD(samplerCubeArray),
        TEST_PAIR_KEYWORD(samplerCubeArrayShadow),
        TEST_PAIR_KEYWORD(isamplerCubeArray),
        TEST_PAIR_KEYWORD(usamplerCubeArray),
        TEST_PAIR_KEYWORD(image1D),
        TEST_PAIR_KEYWORD(iimage1D),
        TEST_PAIR_KEYWORD(uimage1D),
        TEST_PAIR_KEYWORD(image2D),
        TEST_PAIR_KEYWORD(iimage2D),
        TEST_PAIR_KEYWORD(uimage2D),
        TEST_PAIR_KEYWORD(image3D),
        TEST_PAIR_KEYWORD(iimage3D),
        TEST_PAIR_KEYWORD(uimage3D),
        TEST_PAIR_KEYWORD(image2DRect),
        TEST_PAIR_KEYWORD(iimage2DRect),
        TEST_PAIR_KEYWORD(uimage2DRect),
        TEST_PAIR_KEYWORD(imageCube),
        TEST_PAIR_KEYWORD(iimageCube),
        TEST_PAIR_KEYWORD(uimageCube),
        TEST_PAIR_KEYWORD(imageBuffer),
        TEST_PAIR_KEYWORD(iimageBuffer),
        TEST_PAIR_KEYWORD(uimageBuffer),
        TEST_PAIR_KEYWORD(image1DArray),
        TEST_PAIR_KEYWORD(iimage1DArray),
        TEST_PAIR_KEYWORD(uimage1DArray),
        TEST_PAIR_KEYWORD(image2DArray),
        TEST_PAIR_KEYWORD(iimage2DArray),
        TEST_PAIR_KEYWORD(uimage2DArray),
        TEST_PAIR_KEYWORD(imageCubeArray),
        TEST_PAIR_KEYWORD(iimageCubeArray),
        TEST_PAIR_KEYWORD(uimageCubeArray),
        TEST_PAIR_KEYWORD(image2DMS),
        TEST_PAIR_KEYWORD(iimage2DMS),
        TEST_PAIR_KEYWORD(uimage2DMS),
        TEST_PAIR_KEYWORD(image2DMSArray),
        TEST_PAIR_KEYWORD(iimage2DMSArray),
        TEST_PAIR_KEYWORD(uimage2DMSArray),
        TEST_PAIR_KEYWORD(struct),    
    };

    std::stringstream inputProgram;
    for(const auto& keywordPair: keywords)
    {
        inputProgram << " " << keywordPair.first << " ";
    }

    // Count of read tokens using Lexer
    size_t readTokensCount = 0;

    auto inputProgramString = inputProgram.str();
    auto lex = glsl::lexer(inputProgramString.c_str());

    // Read all non-empty tokens from stream
    for(auto token = lex.read(); token.getType() != glsl::kType_eof; readTokensCount++, token = lex.read())
    {
       EXPECT_EQ(token.getType(), glsl::kType_keyword);
       EXPECT_EQ(token.getAsKeyword(), keywords[readTokensCount].second);
    }

    // Verify that all keywords in list were read
    EXPECT_EQ(readTokensCount, keywords.size());
}

TEST(Lexer, SkipComment) {
    const std::string program =
        "//hello from the other size\n"
        "void main()\n"
        "{\n"
        "}\n";
    auto lex = glsl::lexer(program.c_str());
    EXPECT_KEYWORD(void)
    EXPECT_IDENTIFIER("main")
    EXPECT_PARENTHESIS_BEGIN()
    EXPECT_PARENTHESIS_END()
    EXPECT_SCOPE_BEGIN()
    EXPECT_SCOPE_END()
}
}

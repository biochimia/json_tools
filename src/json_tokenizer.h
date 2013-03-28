/*
 * Copyright © 2012 Jørgen Lind

 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that copyright
 * notice and this permission notice appear in supporting documentation, and
 * that the name of the copyright holders not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  The copyright holders make no representations
 * about the suitability of this software for any purpose.  It is provided "as
 * is" without express or implied warranty.

 * THE COPYRIGHT HOLDERS DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL THE COPYRIGHT HOLDERS BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
 * OF THIS SOFTWARE.
*/

#ifndef JSON_TOKENIZER_H
#define JSON_TOKENIZER_H

#include <stddef.h>
#include <functional>
#include <list>
#include <string>

namespace JT {

class TokenizerPrivate;

struct Data
{
    Data()
        : temporary(false)
        , data("")
        , size(0)
    {}

    Data(const char *data, size_t size, bool temporary)
        : temporary(temporary)
        , data(data)
        , size(size)
    {}

    bool temporary;
    const char *data;
    size_t size;
};

struct Token
{
    enum Type {
        Error,
        String,
        Ascii,
        Number,
        ObjectStart,
        ObjectEnd,
        ArrayStart,
        ArrayEnd,
        Bool,
        Null
    };

    Token();
    Type name_type;
    Data name;
    Type value_type;
    Data value;
};

enum class Error {
        NoError,
        InvalidToken,
        NeedMoreData,
        ExpectedPropertyName,
        ExpectedDelimiter,
        ExpectedDataToken,
        IlligalPropertyName,
        IlligalPropertyType,
        IlligalDataValue,
        EncounteredIlligalChar,
        CouldNotCreateNode,
        NodeNotFound,
        MissingPropertyName,
        UnknownError
};

class Tokenizer
{
public:
    Tokenizer();
    ~Tokenizer();

    void allowAsciiType(bool allow);
    void allowNewLineAsTokenDelimiter(bool allow);

    void addData(const char *data, size_t size, bool temporary = true);
    size_t registered_buffers() const;
    void registerRelaseCallback(std::function<void(const char *)> callback);

    Error nextToken(Token *next_token);

    Tokenizer(const Tokenizer &other);
    Tokenizer(Tokenizer &&other);
    Tokenizer &operator=(const Tokenizer &rhs);
    Tokenizer &operator=(Tokenizer &&rhs);
private:
    TokenizerPrivate *m_private;
};

class PrinterOption
{
public:
    PrinterOption(bool pretty = false, bool ascii_name = false);

    int shiftSize() const;
    int depth() const;
    bool pretty() const;
    bool ascii_name() const;

    void setDepth(int depth);

    const std::string &prefix() const;
    const std::string &tokenDelimiter() const;
    const std::string &valueDelimiter() const;
    const std::string &postfix() const;

private:
    int m_shift_size;
    int m_depth;
    bool m_pretty;
    bool m_ascii_name;

    std::string m_prefix;
    std::string m_token_delimiter;
    std::string m_value_delimiter;
    std::string m_postfix;
};

class PrintBuffer
{
public:
    bool free() const { return size - used; }
    bool append(const char *data, size_t size);
    char *buffer;
    size_t size;
    size_t used;
};

class Serializer
{
public:
    Serializer();
    Serializer(char *buffer, size_t size);

    void appendBuffer(char *buffer, size_t size);
    void setPrinterOption(const PrinterOption &option);

    bool write(const Token &token);

    void addRequestBufferCallback(std::function<void(Serializer *)> callback);
    const std::list<PrintBuffer> &printBuffers() const;
private:
    void askForMoreBuffers();
    void markCurrentPrintBufferFull();
    bool write(const char *data, size_t size);
    bool write(const std::string &str) { return write(str.c_str(), str.size()); }

    std::list<std::function<void(Serializer *)>> m_request_buffer_callbacks;
    std::list<PrintBuffer *> m_unused_buffers;
    std::list<PrintBuffer> m_all_buffers;

    PrinterOption m_option;
    bool m_first = true;
    bool m_token_start = true;
};

} //Namespace
#endif //JSON_TOKENIZER_H

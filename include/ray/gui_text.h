// +----------------------------------------------------------------------
// | Project : ray.
// | All rights reserved.
// +----------------------------------------------------------------------
// | Copyright (c) 2013-2015.
// +----------------------------------------------------------------------
// | * Redistribution and use of this software in source and binary forms,
// |   with or without modification, are permitted provided that the following
// |   conditions are met:
// |
// | * Redistributions of source code must retain the above
// |   copyright notice, this list of conditions and the
// |   following disclaimer.
// |
// | * Redistributions in binary form must reproduce the above
// |   copyright notice, this list of conditions and the
// |   following disclaimer in the documentation and/or other
// |   materials provided with the distribution.
// |
// | * Neither the name of the ray team, nor the names of its
// |   contributors may be used to endorse or promote products
// |   derived from this software without specific prior
// |   written permission of the ray team.
// |
// | THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// | "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// | LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// | A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// | OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// | SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// | LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// | DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// | THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// | (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// | OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// +----------------------------------------------------------------------
#ifndef _H_GUI_TEXT_H_
#define _H_GUI_TEXT_H_

#include <ray/game_component.h>

_NAME_BEGIN

class Font;
class GUIText final : public GameComponent
{
public:
    typedef std::wstring _Mystr;
    typedef std::wstring::value_type value_type;
    typedef std::wstring::size_type size_type;
    typedef std::wstring::iterator iterator;
    typedef std::wstring::const_iterator const_iterator;
    typedef std::wstring::reverse_iterator reverse_iterator;
    typedef std::wstring::const_reverse_iterator const_reverse_iterator;
    typedef std::wstring::pointer pointer;
    typedef std::wstring::const_pointer const_pointer;
    typedef std::wstring::allocator_type allocator_type;
    typedef std::wstring::reference reference;
    typedef std::wstring::const_reference const_reference;

    typedef std::initializer_list<value_type> initializer_list;

    typedef std::shared_ptr<Font> _MyFont;

    typedef int int_type;

    static const size_type npos = (size_type)(-1);

public:
    GUIText() noexcept;
    GUIText(const _Mystr& text);
    GUIText(_MyFont& font, const _Mystr& text);

    iterator begin() { return _text.begin(); }
    iterator end() { return _text.end(); }

    const_iterator cbegin() const { return _text.begin(); }
    const_iterator cend()   const { return _text.end(); }
    const_iterator end()    const { return _text.end(); }
    const_iterator begin()  const { return _text.begin(); }

    reverse_iterator rbegin() { return _text.rbegin(); }
    reverse_iterator rend() { return _text.rend(); }

    const_reverse_iterator rbegin()  const { return _text.rbegin(); }
    const_reverse_iterator rend()    const { return _text.rend(); }
    const_reverse_iterator crbegin() const { return _text.rbegin(); }
    const_reverse_iterator crend()   const { return _text.rend(); }

    size_type size()     const noexcept { return _text.size(); }
    size_type length()   const noexcept { return _text.length(); }
    size_type max_size() const noexcept { return _text.max_size(); }
    size_type capacity() const noexcept { return _text.capacity(); }

    void resize(size_type n) { _text.resize(n); }
    void resize(size_type n, value_type c) { _text.resize(n, c); }

    void reserve(size_type n = 0) { _text.reserve(n); }

    void clear() noexcept { _text.clear(); }
    void shrink_to_fit() { _text.shrink_to_fit(); }

    bool empty() const noexcept { return _text.empty(); }

    value_type& at(size_type pos) { return _text.at(pos); }
    const value_type& at(size_type pos) const { return _text.at(pos); }

    value_type& back() { return _text.back(); }
    value_type& front() { return _text.front(); }

    const value_type& back()  const { return _text.back(); }
    const value_type& front() const { return _text.front(); }

    GUIText& append(const _Mystr& s) { _text.append(s); return *this; }
    GUIText& append(const _Mystr& s, size_t subpos, size_t sublen) { _text.append(s, subpos, sublen); }
    GUIText& append(const GUIText& s) { _text.append(s._text); return *this; }
    GUIText& append(const GUIText& s, size_t subpos, size_t sublen) { _text.append(s._text, subpos, sublen); }
    GUIText& append(const_pointer s) { _text.append(s); return *this; }
    GUIText& append(const_pointer s, size_type length) { _text.append(s, length); return *this; }
    GUIText& append(size_type n, value_type c) { _text.append(n, c); return *this; }
    GUIText& append(initializer_list il) { _text.append(il); return *this; }
    template<class InputIterator>
    GUIText& append(InputIterator first, InputIterator last) { _text.append(first, last); return *this; }

    void push_back(value_type c) { _text.push_back(c); }
    //void pop_back() { _text.pop_back(); }

    GUIText& assign(const _Mystr& s) { _text.assign(s); return *this; }
    GUIText& assign(const _Mystr& s, size_type subpos, size_type sublen) { _text.assign(s, subpos, sublen); return *this; }
    GUIText& assign(const GUIText& s) { _text.assign(s._text); return *this; }
    GUIText& assign(const GUIText& s, size_type subpos, size_type sublen) { _text.assign(s._text, subpos, sublen); return *this; }
    GUIText& assign(const_pointer s) { _text.assign(s); return *this; }
    GUIText& assign(const_pointer s, size_type length) { _text.assign(s, length); return *this; }
    GUIText& assign(size_type n, value_type c) { _text.assign(n, c); return *this; }
    GUIText& assign(initializer_list il) { _text.assign(il); return *this; }
    template<class InputIterator>
    GUIText& assign(InputIterator first, InputIterator last) { _text.assign(first, last); return *this; }

    GUIText& insert(size_type off, const _Mystr& s) { _text.insert(off, s); return *this; }
    GUIText& insert(size_type off, const _Mystr& s, size_type subpos, size_type sublen) { _text.insert(off, s, subpos, sublen); };
    GUIText& insert(size_type off, const_pointer s) { _text.insert(off, s); return *this; }
    GUIText& insert(size_type off, const_pointer s, size_type n) { _text.insert(off, s, n); return *this; };
    GUIText& insert(size_type off, size_type n, value_type c) { _text.insert(off, n, c); return *this; };
    template<typename InputIterator>
    iterator insert(iterator p, InputIterator first, InputIterator last) { return _text.insert(p, first, last); };

    GUIText& replace(iterator i1, iterator i2, const _Mystr& s) { _text.replace(i1, i2, s); return *this; }
    GUIText& replace(iterator i1, iterator i2, const_pointer s) { _text.replace(i1, i2, s); return *this; }
    GUIText& replace(iterator i1, iterator i2, size_type n, value_type c) { _text.replace(i1, i2, n, c); return *this; }
    GUIText& replace(iterator i1, iterator i2, const_pointer s, size_type n) { _text.replace(i1, i2, s, n); return *this; }
    GUIText& replace(size_type off, size_type length, const _Mystr& s) { _text.replace(off, length, s); return *this; }
    GUIText& replace(size_type pos, size_type len, const _Mystr& s, size_type subpos, size_type sublen) { _text.replace(pos, len, s, subpos, sublen); return *this; }
    GUIText& replace(size_type off, size_type length, const_pointer s) { _text.replace(off, length, s); return *this; }
    GUIText& replace(size_type pos, size_type len, const_pointer s, size_type n) { _text.replace(pos, len, s, n); return *this; }
    GUIText& replace(size_type pos, size_type len, size_type n, value_type c) { _text.replace(pos, len, n, c); return *this; }
    template<typename InputIterator>
    GUIText& replace(iterator i1, iterator i2, InputIterator first, InputIterator last) { _text.replace(i1, i2, first, last); return *this; }

    //GUIText& erase(const_iterator p) { _text.erase(p); return *this; }
    //GUIText& erase(const_iterator first, const_iterator last) { _text.erase(first, last); return *this; }
    //GUIText& erase(size_type off = 0, size_type count = npos) { _text.erase(off, count); return *this; }

    void swap(GUIText& other) { _text.swap(other._text); }

    const_pointer c_str() const noexcept { return _text.c_str(); }
    const_pointer data()  const noexcept { return _text.data(); }

    allocator_type get_allocator() const noexcept { return _text.get_allocator(); }

    size_type copy(pointer s, size_type len, size_type pos = 0) const { return _text.copy(s, len, pos); }

    size_type find(const _Mystr& s, size_type pos = 0) const noexcept { return _text.find(s, pos); }
    size_type find(const GUIText& s, size_type pos = 0) const noexcept { return _text.find(s._text, pos); }
    size_type find(const_pointer s, size_type pos = 0) const { return _text.find(s, pos); }
    size_type find(const_pointer s, size_type pos, size_type n) const { return _text.find(s, pos, n); }
    size_type find(value_type c, size_type pos = 0) const noexcept { return _text.find(c, pos); }

    size_type rfind(const _Mystr& s, size_type pos = npos) const noexcept { return _text.rfind(s, pos); }
    size_type rfind(const GUIText& s, size_type pos = npos) const noexcept { return _text.rfind(s._text, pos); }
    size_type rfind(const_pointer s, size_type pos = npos) const { return _text.rfind(s, pos); }
    size_type rfind(const_pointer s, size_type pos, size_type n) const { return _text.rfind(s, pos); }
    size_type rfind(value_type c, size_type pos = npos) const noexcept { return _text.rfind(c, pos); }

    size_type find_first_of(const _Mystr& s, size_type pos = 0) const noexcept { return _text.find_first_of(s, pos); }
    size_type find_first_of(const GUIText& s, size_type pos = 0) const noexcept { return _text.find_first_of(s._text, pos); }
    size_type find_first_of(const_pointer s, size_type pos = 0) const { return _text.find_first_of(s, pos); }
    size_type find_first_of(const_pointer s, size_type pos, size_type n) const { return _text.find_first_of(s, pos); }
    size_type find_first_of(value_type c, size_type pos = 0) const noexcept { return _text.find_first_of(c, pos); }

    size_type find_last_of(const _Mystr& s, size_type pos = npos) const noexcept { return _text.find_last_of(s, pos); }
    size_type find_last_of(const GUIText& s, size_type pos = npos) const noexcept { return _text.find_last_of(s._text, pos); }
    size_type find_last_of(const_pointer s, size_type pos = npos) const { return _text.find_last_of(s, pos); }
    size_type find_last_of(const_pointer s, size_type pos, size_type n) const { return _text.find_last_of(s, pos); }
    size_type find_last_of(value_type c, size_type pos = npos) const noexcept { return _text.find_last_of(c, pos); }

    size_type find_first_not_of(const _Mystr& s, size_type pos = 0) const noexcept { return _text.find_first_not_of(s, pos); }
    size_type find_first_not_of(const GUIText& s, size_type pos = 0) const noexcept { return _text.find_first_not_of(s._text, pos); }
    size_type find_first_not_of(const_pointer s, size_type pos = 0) const { return _text.find_first_not_of(s, pos); }
    size_type find_first_not_of(const_pointer s, size_type pos, size_type n) const { return _text.find_first_not_of(s, pos); }
    size_type find_first_not_of(value_type c, size_type pos = 0) const noexcept { return _text.find_first_not_of(c, pos); }

    size_type find_last_not_of(const _Mystr& s, size_type pos = npos) const noexcept { return _text.find_last_not_of(s, pos); }
    size_type find_last_not_of(const GUIText& s, size_type pos = npos) const noexcept { return _text.find_last_not_of(s._text, pos); }
    size_type find_last_not_of(const_pointer s, size_type pos = npos) const { return _text.find_last_not_of(s, pos); }
    size_type find_last_not_of(const_pointer s, size_type pos, size_type n) const { return _text.find_last_not_of(s, pos); }
    size_type find_last_not_of(value_type c, size_type pos = npos) const noexcept { return _text.find_last_not_of(c, pos); }

    _Mystr substr(size_type pos = 0, size_type len = npos) const { return _text.substr(pos, len); }

    int_type compare(const _Mystr& s) const noexcept { return _text.compare(s); }
    int_type compare(size_type pos, size_type len, const _Mystr& s) const { return _text.compare(pos, len, s); }
    int_type compare(size_type pos, size_type len, const _Mystr& s, size_type subpos, size_type sublen) const { return _text.compare(pos, len, s, subpos, sublen); }
    int_type compare(const_pointer s) const { return _text.compare(s); }
    int_type compare(size_type pos, size_type len, const_pointer s) const { return _text.compare(pos, len, s); }
    int_type compare(size_type pos, size_type len, const_pointer s, size_type n) const { return _text.compare(pos, len, s, n); }

    GUIText& operator+=(const _Mystr& s) { _text += s; return *this; }
    GUIText& operator+=(const_pointer s) { _text += s; return *this; }
    GUIText& operator+=(value_type c) { _text += c; return *this; }
    GUIText& operator+=(initializer_list il) { _text += il; return *this; }

    GUIText& operator=(const _Mystr& s) { _text = s; return *this; }
    GUIText& operator=(const_pointer s) { _text = s; return *this; }
    GUIText& operator=(value_type c) { _text = c; return *this; }
    GUIText& operator=(initializer_list il) { _text = il; return *this; }

    value_type& operator[] (size_type pos) { return _text[pos]; }
    const value_type& operator[] (size_type pos) const { return _text[pos]; }

    void setFont(_MyFont font) { _font = font; }

    void generate();

private:
    GUIText(const GUIText& text) = delete;
    GUIText& operator=(const GUIText& text) = delete;
    ~GUIText() noexcept;

    friend class GameObject;

private:

    _Mystr _text;

    _MyFont _font;
};

_NAME_END

#endif
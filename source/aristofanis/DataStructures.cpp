#include "DataStructures.h"

namespace helper_functions {

    void zero_order_complement(uint8_t idx)  {
        if(idx == 4) return 4;
        return 3-idx;
    }

    // Based on Table 36, page 49
    void first_order_complement(uint8_t idx) {
        if(idx <= 3) return 3-idx;
        if(idx <= 5) return 5-idx  + 4; 
        if(idx <= 7) return idx;
        if(idx <= 9) return 9-idx + 8;
        if(idx <= 13) return 13-idx + 10;
        return idx;
    }

    // Supported symbols
    void initialize_supported_symbols(data_structures::expandable_array<std::string> &supported_symbols) {
        supported_symbols = data_structures::expandable_array<std::string>(2, "");
        supported_symbols[0] = "ACGTN";
        supported_symbols[1] = "ACGTRYSWKMBDHVN-";
    }

}

namespace data_structures {

    template<typename T> 
    expandable_array::expandable_array()    
        { vec = std::vector<T>(); }
    template<typename T> 
    expandable_array::expandable_array(size_t n)    
        { vec = std::vector<T>(n, T()); }
    template<typename T> 
    expandable_array::expandable_array(size_t n, const &T val)  
        { vec = std::vector<T>(n, val); }
    template<typename T> 
    expandable_array::expandable_array(expandable_array::const_iterator l, expandable_array::const_iterator r)  
        { vec = std::vector<T>(l, r); }

    template<typename T>
    T& expandable_array::operator[](size_t idx) {
        while(vec.size() <= idx)
            vec.push_back(T());
        return vec[idx];
    }

    template<typename T>
    const T& expandable_array::operator[](size_t idx) const {
        while(vec.size() <= idx)
            vec.push_back(T());
        return vec[idx];
    }

    template<typename T>
    size_t  expandable_array::size()        { return vec.size(); }
    template<typename T>
    void    expandable_array::clear()       { vec.clear(); }
    template<typename T>
    bool    expandable_array::empty()       { return vec.empty(); }

    template<typename T>
    expandable_array::iterator expandable_array::begin()                { return vec.begin(); }
    template<typename T>
    expandable_array::iterator expandable_array::end()                  { return vec.end(); }
    template<typename T>
    expandable_array::const_iterator expandable_array::begin() const    { return vec.begin(); }
    template<typename T>
    expandable_array::const_iterator expandable_array::end() const      { return vec.end(); }

    template<typename T>
    expandable_array::reverse_iterator expandable_array::rbegin()               { return vec.rbegin(); }
    template<typename T>
    expandable_array::reverse_iterator expandable_array::rend()                 { return vec.rend(); }
    template<typename T>
    expandable_array::const_reverse_iterator expandable_array::rbegin() const   { return vec.rbegin(); }
    template<typename T>
    expandable_array::const_reverse_iterator expandable_array::rend() const     { return vec.rend(); }

    
    COP::COP()                      {}
    RLE::RLE()                      {}
    CABAC_METHOD_0::CABAC_METHOD_0  {}
    CABAC_METHOD_1::CABAC_METHOD_1  {}
    X4::X4                          {}

    COP::COP(uint16_t i)                                        { assert(0); }
    RLE::RLE(uint16_t i, uint64_t nos)                          { assert(0); }
    CABAC_METHOD_0::CABAC_METHOD_0(uint16_t i, uint64_t nos)    { assert(0); }
    CABAC_METHOD_1::CABAC_METHOD_1(uint16_t i, uint64_t nos)    { assert(0); }
    X4::X4(uint16_t i, uint64_t nos)                            { assert(0); }

    CAT::CAT()                          {}
    CAT::CAT(uint16_t i, uint64_t nos)  { decoded_tokens[i] = expandable_array<uint8_t>(nos); }

    decode_tokentype_sequence::decode_tokentype_sequence()  {}

    decode_tokentype_sequence::decode_tokentype_sequence(uint16_t i, uint8_t method_ID, uint64_t nos) {
        if(method_ID == 1)          cat = CAT(i, nos);
        else if(method_ID == 2)     rle = RLE(i, nos);
        else if(method_ID == 3)     cm0 = CABAC_METHOD_0(i, nos);
        else if(method_ID == 4)     cm1 = CABAC_METHOD_1(i, nos);
        else if(method_ID == 5)     x4  = X4(i, nos);
        else assert(0);
    }

    encoded_tokentype_sequence::encoded_tokentype_sequence()                                                {}
    encoded_tokentype_sequence::encoded_tokentype_sequence(uint16_t i, uint8_t method_ID)                   { assert(method_ID == 0); cop = COP(i); } // will throw exception
    encoded_tokentype_sequence::encoded_tokentype_sequence(uint16_t i, uint8_t method_ID, uint64_t nos)     { assert(method_ID != 0); dec_tt_seq = decode_tokentype_sequence(i, method_ID, nos); }  

    block_payload::block_payload()                          {}
    block_payload::block_payload(uint8_t descriptor_ID)     { /* TODO */ }

}
# TODO

---

## Current

### Brian

### Daniel/Jannis

- [ ] Transcode part 1 en-/decapsulator fom Java to C++ (standalone + library)

### Fabian

- [ ] Implement local assembly encoder

### Jan

- [x] Clean repository with: https://rtyley.github.io/bfg-repo-cleaner/
- [ ] Add boilerplate to all source code files
- [ ] Update setting on github.com and codecov.io and travis.com and travis.org
- [ ] Set up GitHub pages at: https://mitogen.github.io/genie/
- [ ] Set up Doxygen documentation at: https://mitogen.github.io/genie/doc/
- [ ] Re-enable util-tests in ci/run-tests.sh
- [ ] Set up code coverage system (publish the reports at https://mitogen.github.io/genie/codecov/ and https://codecov.io/gh/mitogen/genie/)
- [ ] Check whether everything builds nicely on Travis CI
  - [ ] Fix compilation warnings
  - [ ] Enable all tests in .travis.yml
  - [ ] Add autopep8-pycodestyle.sh to util/before-commit.sh
  - [ ] Fix clang-tidy warnings
- [ ] Check file and folder names in the src/ and test/ trees
- [ ] Don't let genie produce genie.log by default
- [ ] Rename command line options to something like --input-file instead of --input_file or --input-file-path (this applies to *all* applications)
- [ ] Check comments
  - [ ] Sentences should start with an uppercase letter and end with a full stop.
  - [ ] Statements should start with an uppercase letter and not end with a full stop.
- [ ] Check use and linking of OpenMP
- [ ] Clean up exceptions (use only a simple class derived from std::exception - without all the special things)
- [ ] Implement a unified logging solution
- [ ] Check for correct usage of assertions versus exceptions
- [ ] Implement C++ Core Guideline C.21 ("If you define or =delete any default operation, define or =delete them all")
- [ ] Make destructors virtual in all classes that something is being derived from (use override in derived classes)
- [ ] Unify I/O
  - [ ] Bitstream I/O should be done using HM's bitstream classes adapted for 64 bits
  - [ ] "Ordinary" file I/O (for e.g. SPRING's temporary files) should be done using the C++ standard library
- [ ] Use only numpy arrays in Python interfaces (just as in the example "c-and-python")

### Josh

- [ ] Write unit tests

### Junaid

- [ ] Investigate GABAC roundtrip ("u(32) issue")
- [ ] Check whether adaptive entropy coding works in conformance tests

### Shubham

- [ ] Make Genie's SPRING mode conformant to MPEG-G
  - [ ] Write a parameter set and an access unit - which only contains e.g. the encoded read IDs - to a file using the functionality from the format library.
  - [ ] Try to decode that with the reference software.

### Yeremia

## When everything is working

- [ ] Clean repository with: https://rtyley.github.io/bfg-repo-cleaner/
- [ ] Check conformance to doc/development-guidelines.md
- [ ] Check everything with Valgrind
- [ ] Resolve FIXMEs and TODOs
- [ ] Update README.md
- [ ] Update doc/gabac.md
- [ ] Check LICENSE
- [ ] Design social media image (for GitHub)

## Futuristic things

- [ ] Replace thirdparty/filesystem/ with std::filesystem (C++17-compliant compilers are needed for that)

## SPRING integration

### encoding_parameters()

```
dataset_type = 0
alphabet_ID = 0
read_length = 0 (we use rlen)
number_of_template_segments_minus1 = 1 (single-/paired-end)
max_au_data_unit_size = 0 (leave unspecified)
pos_40_bits_flag = 1 (just to be save)
qv_depth = 0 or 1
as_depth = 0
num_classes = 1
class_ID[0] = 6
class_specific_dec_cfg_flag[i] = 0
num_groups = 0
multiple_alignments_flag = 0
spliced_reads_flag = 0
multiple_signature_base = 0
qv_coding_mode = 1
qvps_flag = 0
qvps_preset_ID = 0
qv_reverse_flag = 1
crps_flag = 1
```

#### parameter_set_crps()

```
cr_alg_ID = 4 (global assembly)
```

### Notes

- In the AU header, to get the reads_count, the easiest way is the size of the rcomp descriptor stream (subseq_vector[1][0].size()). num_blocks is the number of descriptors used, i.e., number of values first index of subseq_vector takes + 1 each for QV and read ID.
- To put the compressed block payloads into AUs, the easiest way would be write the compressed read descriptors to temporary files in generate-read-streams.cc, then write the compressed quality and id descriptors to temporary files in reorder-compress-quality-id.cc. Finally you can have a separate combining step to combine these into the AU structure.
- Note that the vector tokens[i][j][] in SPRING corresponds to vector decoded_tokens[i*16+j][] in the standard (section 10.4.19).
- For token type we need to be a bit careful. The decoded_tokens is treated as a byte array in the standard. Depending on the value of j, we either extract 1 byte from this array (when j = 0, 1, 2, 3, 5) or we extract a 4 byte integer from this (when j = 4). In my tokens array, everything is int64, with value within 255 when j = 0, 1, 2, 3, 5 and value within 2^32-1 when j = 4. So we should store things with correct number of bytes.
- For quality values, we’ll need to subtract 33 from the ASCII value before compression, since the decompression process in 7.3.2.2.1.1 adds 33 after compression.
- While the global assembly in the reference decoder gets implemented, we can start testing the ureads_flag in SPRING which allows us to test the read ID tokentype compression.
- As we put things in the conformant format, we should also update the SPRING decompressor. Since the SPRING decompressor has additional capabilities like matching pairs, it will be good to keep it working and not rely solely on the reference decoder.
- SPRING uses BBHash as a hash function (boo-phf.h) which uses fopen(). Just wanted to let you know since Jan asked me about any file handling other than std::fstream. This is someone else’s code and I have not touched it at all. Feel free to change it to std::fstream if needed.

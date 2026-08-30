use native_test

say add 20 22
say square 4.0
say not_bool false
say echo "native text"

data = bytes "hello bytes"
say bytes_len data
copy = bytes_echo data
say copy.len

try
    bad = native_error 1
    say bad
else err
    say err.kind

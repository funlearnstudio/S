try
    text = read "this-file-does-not-exist.txt"
    say text
else err
    say err.kind
    say err

make validate score
    if score < 0
        fail "Invalid score"
    give score

try
    say validate -1
else err
    say err

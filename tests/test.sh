#!/bin/bash

set -e

VIBRA_CLI=vibra

function pass() {
    printf "\e[32m%s\e[0m\n" "$1"
}

function fail() {
    printf "\e[31m%s\e[0m\n" "$1"
}

function info() {
    printf " - %s\t" "$1"
}

function download_audio() {
    local url=$1
    local format=$2
    local output=$3
    yt-dlp -x --audio-format "$format" -o "$output" "$url" > /dev/null
}

function recognize_audio() {
    local file=$1
    $VIBRA_CLI --recognize --file "$file" | jq .track.title -r
}

function check_title() {
    local expected=$1
    local actual=$2
    if [ "$actual" != "$expected" ]; then
        fail "Failed to recognize the track (Expected: $expected, Got: $actual)"
        exit 1
    fi
}

function test_audio_file() {
    local url=$1
    local format=$2
    local expected_title=$3
    local file="/tmp/test.$format"

    info "Testing $format file..."
    download_audio "$url" "$format" "$file"

    local title=$(recognize_audio "$file")
    check_title "$expected_title" "$title"
}

function test_stdin() {
    info "Testing stdin..."
    local url=$1
    local format=$2
    local expected_title=$3
    local file="/tmp/test_stdin.$format"

    download_audio "$url" "$format" "$file"

    local title
    title=$(ffmpeg -i "$file" -f s24le -ac 2 -ar 48000 - 2>/dev/null | \
        $VIBRA_CLI --recognize --seconds 5 --rate 48000 --channels 2 --bits 24 | \
        jq .track.title -r)

    check_title "$expected_title" "$title"
}

function run_tests() {
    test_audio_file "https://www.youtube.com/watch?v=QkF3oxziUI4" "wav" "Stairway to Heaven"
    pass "passed"

    test_audio_file "https://www.youtube.com/watch?v=n4RjJKxsamQ" "mp3" "Wind of Change"
    pass "passed"

    test_audio_file "https://www.youtube.com/watch?v=qQzdAsjWGPg" "flac" "My Way"
    pass "passed"

    test_stdin "https://www.youtube.com/watch?v=mQER0A0ej0M" "wav" "Hey Jude"
    pass "passed"

    echo
    pass "All tests passed"
}

run_tests
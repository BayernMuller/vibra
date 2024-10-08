#!/bin/bash

set -e

VIBRA_CLI=vibra
FAILED=0

function pass() {
    printf "\e[32m%s\e[0m\n" "$1"
}

function fail() {
    printf "\e[31m%s\e[0m\n" "$1"
    FAILED=1
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
    else
        pass "passed!"
    fi
}

function test_audio_file() {
    local url=$1
    local format=$2
    local expected_title=$3
    local file="/tmp/test.$format"

    info "Testing $format file..."
    download_audio "$url" "$format" "$file"

    local title
    title=$(recognize_audio "$file")
    check_title "$expected_title" "$title"
}

function test_raw_pcm() {
    info "Testing raw PCM data..."
    echo

    local url=$1
    local format=$2
    local expected_title=$3
    local file="/tmp/test_stdin.$format"

    download_audio "$url" "$format" "$file"

    for type in signed float; do
        for bit in 16 24 32 64; do
            for rate in 44100 48000; do
                for channels in 1 2; do
                    if [[ "$type" == "float" && "$bit" -lt 32 ]] || [[ "$type" == "signed" && "$bit" -gt 32 ]]; then
                        continue 
                    fi
                    codec="${type:0:1}${bit}le"
                    info "   $codec $(echo $rate / 1000 | bc)kHz ${channels}ch..."

                    local title
                    title=$(ffmpeg -i "$file" -f "$codec" -ac $channels -ar $rate - 2>/dev/null | \
                        $VIBRA_CLI --recognize --seconds 5 --rate $rate --channels $channels --bits $bit | \
                        jq .track.title -r)
                    check_title "$expected_title" "$title"
                    sleep 3
                done
            done
        done
    done
}


function run_tests() {
    echo
    echo "Running vibra tests..."
    echo

    test_audio_file "https://www.youtube.com/watch?v=QkF3oxziUI4" "wav" "Stairway to Heaven"
    test_audio_file "https://www.youtube.com/watch?v=n4RjJKxsamQ" "mp3" "Wind of Change"
    test_audio_file "https://www.youtube.com/watch?v=qQzdAsjWGPg" "flac" "My Way"
    test_raw_pcm "https://www.youtube.com/watch?v=mQER0A0ej0M" "wav" "Hey Jude"
    
    echo
    if [ $FAILED -eq 1 ]; then
        fail "Some tests failed!"
    else
        pass "All tests passed!"
    fi
}

run_tests


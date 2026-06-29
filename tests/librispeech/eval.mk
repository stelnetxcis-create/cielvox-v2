PYTHON = python

STELNET_PREFIX = ../../
STELNET_MODEL = tiny

STELNET_CLI = $(STELNET_PREFIX)build/bin/stelnet
STELNET_FLAGS = --no-prints --language en --output-txt

# You can create eval.conf to override the STELNET_* variables
# defined above.
-include eval.conf

# This follows the file structure of the LibriSpeech project.
AUDIO_SRCS = $(sort $(wildcard LibriSpeech/*/*/*/*.flac))
TRANS_TXTS = $(addsuffix .txt, $(AUDIO_SRCS))

# We output the evaluation result to this file.
DONE = $(STELNET_MODEL).txt

all: $(DONE)

$(DONE): $(TRANS_TXTS)
	$(PYTHON) eval.py > $@.tmp
	mv $@.tmp $@

# Note: This task writes to a temporary file first to
# create the target file atomically.
%.flac.txt: %.flac
	$(STELNET_CLI) $(STELNET_FLAGS) --model $(STELNET_PREFIX)models/ggml-$(STELNET_MODEL).bin --file $^ --output-file $^.tmp
	mv $^.tmp.txt $^.txt

archive:
	tar -czf $(STELNET_MODEL).tar.gz --exclude="*.flac" LibriSpeech $(DONE)

clean:
	@rm -f $(TRANS_TXTS)
	@rm -f $(DONE)

.PHONY: all clean

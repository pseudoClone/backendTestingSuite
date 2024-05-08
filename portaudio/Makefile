EXEC = cppaudiocapture

CLIB = -I include/ -lrt -lasound -ljack -pthread -lportaudio 
# %^ - Source
# $@ - Deps

$(EXEC): main.cpp
	g++ -o $@ $^ $(CLIB)
clean:
	rm -rf $(EXEC)
.PHONY: clean

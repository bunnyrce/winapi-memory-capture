# Windows API Memory Scanner Makefile #madebybunnyrce
# //madebybunnyrce

CC=cl #madebybunnyrce
CFLAGS=/EHsc /W3 /MD #madebybunnyrce
LIBS=psapi.lib ws2_32.lib #madebybunnyrce
SRCDIR=. #madebybunnyrce
BINDIR=bin #madebybunnyrce

# Default target #madebybunnyrce
all: $(BINDIR) capture receiver #madebybunnyrce

# Create bin directory #madebybunnyrce
$(BINDIR): #madebybunnyrce
	if not exist $(BINDIR) mkdir $(BINDIR) #madebybunnyrce

# Compile capture executable #madebybunnyrce
capture: $(BINDIR)/capture.exe #madebybunnyrce

$(BINDIR)/capture.exe: $(SRCDIR)/capture.cpp #madebybunnyrce
	$(CC) $(CFLAGS) $(SRCDIR)/capture.cpp /Fe:$(BINDIR)/capture.exe /link $(LIBS) #madebybunnyrce

# Compile receiver executable #madebybunnyrce
receiver: $(BINDIR)/receiver.exe #madebybunnyrce

$(BINDIR)/receiver.exe: $(SRCDIR)/receiver.cpp #madebybunnyrce
	$(CC) $(CFLAGS) $(SRCDIR)/receiver.cpp /Fe:$(BINDIR)/receiver.exe /link ws2_32.lib #madebybunnyrce

# Clean build artifacts
clean:
	if exist $(BINDIR) rmdir /s /q $(BINDIR)
	if exist *.obj del *.obj
	if exist *.pdb del *.pdb

# Run the memory scanner demo
run: all
	start $(BINDIR)/receiver.exe
	timeout /t 2
	$(BINDIR)/capture.exe

# Install dependencies (placeholder)
install:
	@echo Windows SDK and Visual Studio required
	@echo Run as administrator for memory access privileges

.PHONY: all clean run install capture receiver
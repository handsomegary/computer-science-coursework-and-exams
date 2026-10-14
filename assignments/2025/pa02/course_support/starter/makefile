CXX = g++
CXXFLAGS = -O2 -std=c++11 -Wall
TIME = /usr/bin/time -f "Time: %E (User: %U, Sys: %S)"

PA2_SRC = PA2/PA2.cpp
PA2_EXE = PA2/PA2

PA2_BONUS_SRC = PA2_bonus/PA2_bonus.cpp
PA2_BONUS_EXE = PA2_bonus/PA2_bonus
C = PublicCase

# make all (編譯所有程式)
all: $(PA2_EXE)
$(PA2_EXE): $(PA2_SRC)
	$(CXX) $(CXXFLAGS) $< -o $@

all_bonus: $(PA2_BONUS_EXE)
$(PA2_BONUS_EXE): $(PA2_BONUS_SRC)
	$(CXX) $(CXXFLAGS) $< -o $@


# ==================== RUN NORMAL CASES ====================
run_pa2: $(PA2_EXE)
	@if [ -z "$(CASE)" ]; then \
		echo "=== Running ALL test cases ===" ; \
		for f in PA2/Testcase/PA2_$(C)*.txt; do \
			$(MAKE) --no-print-directory run_one FILE=$$f; \
		done; \
	else \
		f=$$(ls PA2/Testcase/PA2_$(C)$(CASE)_*s.txt 2>/dev/null || echo ""); \
		if [ -z "$$f" ]; then \
			echo "Test case $(CASE) not found" ; \
		else \
			$(MAKE) --no-print-directory run_one FILE=$$f; \
		fi; \
	fi

run_one: $(PA2_EXE)
	@name=$$(basename $(FILE) .txt); \
	limit=$$(echo $$name | grep -oP '\d+(?=s$$)'); \
	out=PA2/Result/$$name.out; \
	echo "=== Running $$name (target: $$limit s) ===" ; \
	start=$$(date +%s); \
	timeout 120s ./$(PA2_EXE) < $(FILE) > $$out; \
	status=$$?; \
	end=$$(date +%s); \
	elapsed=$$((end-start)); \
	if [ $$status -eq 124 ]; then \
		echo "Result: TIMEOUT (> 2 min)" ; \
	elif [ $$elapsed -le $$limit ]; then \
		echo "Result: PASS (time=$$elapsed s, limit=$$limit s)" ; \
	else \
		echo "Result: FAILED (time=$$elapsed s, limit=$$limit s)" ; \
	fi

# ==================== RUN BONUS CASES ====================
run_pa2_bonus: $(PA2_BONUS_EXE)
	@if [ -z "$(CASE)" ]; then \
		echo "=== Running ALL bonus test cases ===" ; \
		for f in PA2_bonus/Testcase/PA2_bonus_$(C)*.txt; do \
			$(MAKE) --no-print-directory run_one_bonus FILE=$$f; \
		done; \
	else \
		f=$$(ls PA2_bonus/Testcase/PA2_bonus_$(C)$(CASE)_*s.txt 2>/dev/null || echo ""); \
		if [ -z "$$f" ]; then \
			echo "Bonus test case $(CASE) not found" ; \
		else \
			$(MAKE) --no-print-directory run_one_bonus FILE=$$f; \
		fi; \
	fi

run_one_bonus: $(PA2_BONUS_EXE)
	@name=$$(basename $(FILE) .txt); \
	limit=$$(echo $$name | grep -oP '\d+(?=s$$)'); \
	out=PA2_bonus/Result/$$name.out; \
	echo "=== Running $$name (target: $$limit s, BONUS) ===" ; \
	start=$$(date +%s); \
	timeout 120s ./$(PA2_BONUS_EXE) < $(FILE) > $$out; \
	status=$$?; \
	end=$$(date +%s); \
	elapsed=$$((end-start)); \
	if [ $$status -eq 124 ]; then \
		echo "Result: TIMEOUT (> 2 min)" ; \
	elif [ $$elapsed -le $$limit ]; then \
		echo "Result: PASS (time=$$elapsed s, limit=$$limit s)" ; \
	else \
		echo "Result: FAILED (time=$$elapsed s, limit=$$limit s)" ; \
	fi

# ==================== CHECKER ====================
CHECKER = checker

check_pa2: $(CHECKER)
	@if [ -z "$(CASE)" ]; then \
		echo "=== Checking ALL test cases ===" ; \
		for f in PA2/Testcase/PA2_$(C)*.txt; do \
			base=$${f/.txt/}; \
			name=$$(basename $$f .txt); \
			echo "=== Checking $$name ===" ; \
			./$(CHECKER) $$f PA2/Result/$$name.out; \
		done; \
	else \
		f=$$(ls PA2/Testcase/PA2_$(C)$(CASE)_*s.txt 2>/dev/null || echo ""); \
		if [ -z "$$f" ]; then \
			echo "Case $(CASE) not found"; \
		else \
			base=$${f/.txt/}; \
			name=$$(basename $$f .txt); \
			echo "=== Checking $$name ===" ; \
			./$(CHECKER) $$f PA2/Result/$$name.out; \
		fi; \
	fi


check_pa2_bonus: $(CHECKER)
	@if [ -z "$(CASE)" ]; then \
			echo "=== Checking ALL bonus test cases ===" ; \
		for f in PA2_bonus/Testcase/PA2_bonus_$(C)*.txt; do \
			base=$${f/.txt/}; \
			name=$$(basename $$f .txt); \
			echo "=== Checking $$name (BONUS) ===" ; \
			./$(CHECKER) $$f PA2_bonus/Result/$$name.out; \
		done; \
	else \
		f=$$(ls PA2_bonus/Testcase/PA2_bonus_$(C)$(CASE)_*s.txt 2>/dev/null || echo ""); \
		if [ -z "$$f" ]; then \
			echo "Bonus case $(CASE) not found"; \
		else \
			base=$${f/.txt/}; \
			name=$$(basename $$f .txt); \
			echo "=== Checking $$name (BONUS) ===" ; \
			./$(CHECKER) $$f PA2_bonus/Result/$$name.out; \
		fi; \
	fi


# ==================== CLEAN ====================
clean:
	rm -f $(PA2_EXE) $(PA2_BONUS_EXE) PA2/Result/*.out PA2_bonus/Result/*.out 

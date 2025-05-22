#!/bin/bash

ITERATIONS=10000
ARG_DIRECTION="from"
ARG_IP="127.0.0.1" # Diese IP wird blockiert, testet den "denied"-Pfad

# Dummy-Datei direkt im aktuellen Verzeichnis erstellen
DUMMY_LOCAL_DIR="." # Aktuelles Verzeichnis
DUMMY_LOCAL_FILENAME="dummy_benchmark_file.dat"

# Sicherstellen, dass die Datei existiert
touch "$DUMMY_LOCAL_DIR/$DUMMY_LOCAL_FILENAME"

# Argumente für die Skripte
ARG_FILENAME="$DUMMY_LOCAL_FILENAME" # Nur der Dateiname
ARG_FILEPATH="$DUMMY_LOCAL_DIR"      # Pfad zum Verzeichnis (hier das aktuelle)

echo "Testing Bash script (fxpscript.sh, $ITERATIONS iterations, IP should be DENIED)..."
time (
    for ((i=0; i<ITERATIONS; i++)); do
        ./fxpscript.sh "$ARG_DIRECTION" "$ARG_IP" "$ARG_FILENAME" "$ARG_FILEPATH" > /dev/null 2>&1
    done
)
echo "------------------------------------"

echo "Testing C++ program (fxp_checker, $ITERATIONS iterations, IP should be DENIED)..."
time (
    for ((i=0; i<ITERATIONS; i++)); do
        ./fxp_check "$ARG_DIRECTION" "$ARG_IP" "$ARG_FILENAME" "$ARG_FILEPATH" > /dev/null 2>&1
    done
)
echo "===================================="

# Aufräumen
rm -f "$DUMMY_LOCAL_DIR/$DUMMY_LOCAL_FILENAME"
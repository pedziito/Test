#!/bin/bash

echo "════════════════════════════════════════════════"
echo "  JAR File Inspector - Se indhold af JAR"
echo "════════════════════════════════════════════════"
echo ""

if [ ! -f "$1" ]; then
    echo "Brug: ./inspect_jar.sh <sti-til-jar-fil>"
    echo "Eksempel: ./inspect_jar.sh web_injected/injected_FiskeSystem.jar"
    exit 1
fi

JAR_FILE="$1"
echo "📦 Inspicerer: $JAR_FILE"
echo ""

echo "📊 Fil information:"
ls -lh "$JAR_FILE"
echo ""

echo "📋 Indhold af JAR fil:"
echo "────────────────────────────────────────────────"
unzip -l "$JAR_FILE"
echo ""

echo "🔍 Søger efter injecteret kode:"
echo "────────────────────────────────────────────────"
unzip -l "$JAR_FILE" | grep -E "(Debugger|debugger\.config)" && echo "✅ Debugger fundet!" || echo "❌ Debugger ikke fundet"
echo ""

echo "📄 debugger.config indhold:"
echo "────────────────────────────────────────────────"
unzip -p "$JAR_FILE" debugger.config 2>/dev/null || echo "Ingen config fil fundet"
echo ""

echo "✨ JAR filen er klar til brug som Minecraft plugin!"
echo "   Læg den i din servers plugins/ mappe"

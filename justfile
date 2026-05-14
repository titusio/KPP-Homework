
[macos]
watch:
    latexmk -pvc -lualatex -interaction=nonstopmode document.tex

[linux]
watch:
    latexmk -pvc -lualatex -interaction=nonstopmode -e '$pdf_previewer = "start xdg-open"' document.tex

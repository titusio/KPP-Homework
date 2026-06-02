deploy-3_1:
    #!/usr/bin/env bash
    set -e
    rsync -av --exclude='*.o' ex3/input/3_1/ cluster:~/kpp/ex3/3_1/
    JOB=$(ssh cluster "cd ~/kpp/ex3/3_1 && sbatch sbatch.sh" | awk '{print $4}')
    echo "Submitted job $JOB"
    while ssh cluster "squeue -j $JOB -h 2>/dev/null" | grep -q .; do
        echo "Waiting for job $JOB..."; sleep 10
    done
    echo "Job $JOB done, fetching output..."
    scp "cluster:~/kpp/ex3/3_1/out.$JOB" "cluster:~/kpp/ex3/3_1/err.$JOB" ex3/input/3_1/
    echo "--- out.$JOB ---"; cat ex3/input/3_1/out.$JOB
    echo "--- err.$JOB ---"; cat ex3/input/3_1/err.$JOB
    rm ex3/input/3_1/out.* ex3/input/3_1/err.*

[macos]
watch:
    latexmk -pvc -lualatex -interaction=nonstopmode document.tex

[linux]
watch:
    latexmk -pvc -lualatex -interaction=nonstopmode -e '$pdf_previewer = "start xdg-open"' document.tex

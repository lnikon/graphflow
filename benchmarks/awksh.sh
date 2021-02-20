awk 'NR == 1 {line = $0; min = $3}
     NR > 1 && $3 < min {line = $0; min = $3}
     END{print line}' file.txt

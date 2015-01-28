cd "$( dirname "${BASH_SOURCE[0]}" )"

for i in `seq 130 10 150`;
do
  rm -f ../data/GENERATED_random_connect0.8.txt
  python random_graph.py $i 0.8 > ../data/GENERATED_random_connect0.8.txt
  echo $i
  time ../triangle.exe ../data/GENERATED_random_connect0.8.txt
done

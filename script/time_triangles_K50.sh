cd "$( dirname "${BASH_SOURCE[0]}" )"

for i in `seq 60 10 100`;
do
  rm -f ../data/random_connect0.8.txt
  python random_graph.py $i 0.8 > ../data/random_connect0.8.txt
  echo $i
  time ../triangle.exe ../data/random_connect0.8.txt
done

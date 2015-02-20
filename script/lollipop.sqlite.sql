SELECT COUNT(*)
FROM Edges e1, Edges e2, Edges e3, Edges e4, Edges e5, Edges e7, Edges e8
WHERE e1.b = e2.a
  AND e2.b = e3.a
  AND e3.b = e4.a
  AND e4.b = e1.a
  AND e1.b = e5.a
  AND e7.a = e2.a
  AND e7.b = e3.b
  AND e8.a = e3.a
  AND e8.b = e4.b;

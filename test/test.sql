select t.hello as t/* /*this is a eample*/ */,kk,lo from "test" t,teo;
/*just for test */
select a from p where id = o or p = o;
-- this is a comment
select all t.k,a,b + d as b from test t where x = t and tet = o;
SELECT * FROM (SELECT 1 as first,'d' as second,'1992'as third FROM dual) t;
-- big sql
select distinct emp.* from employees emp,
         (select employee_id from employees where first_name = 'Peter') emp1
   where emp.employee_id = emp1.employee_id;
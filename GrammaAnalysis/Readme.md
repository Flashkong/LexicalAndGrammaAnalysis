# 对New和Old的说明

```
begin
  integer k;
  integer function F(n);
    begin
      integer n;
      if n<=0 then F:=1
      else F:=n*F(n-1)
    end;
  read(m);
  k:=F(m);
  write(k)
end
```

new里面对程序进行了修改，将第五行那里也列为了错误。

而old里面则会检测第五行声明的变量的同名变量是不是参数，如果是参数就认为是合法的，所以第五行不认为是错误。
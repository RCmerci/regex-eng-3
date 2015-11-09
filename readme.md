### 正则引擎

由于第一个正则引擎太简陋 ＝ ＝
以及第二个难产


于是开坑第三个，名字就叫 regex-eng-3 好了!

========================================

语法：

```
^                          ->  匹配开始

$                          ->匹配结束

\\d                        ->  [0-9]

\\D                        ->  [^\\d]

\\w                        ->  [a-z0-9_]

\\W                        ->  [^\\w]

\\s                        ->  [ \t\n\r]

\\S                        ->  [^\\s]

.                          ->  [^\n]

|                          ->    或

[...]                      ->  字符集

*                          ->  循环至少0次，贪婪

*?                         ->  循环至少0次，非贪婪

+                          ->  循环至少1次，贪婪

+?                         ->  循环至少1次，非贪婪

{min, max}                 ->  循环min至max次，贪婪 (max为空代表无穷)

{min, max}?                ->  循环min至max次，非贪婪


(...)                      ->  普通括号，用于改变优先级

(?...)                     ->  匿名捕获

(?<name>...)               ->  命名捕获

(=...)                     ->  前向查询

(!...)                     ->  反前向查询

(#<name>...)               ->  表达式命名 

(&<name>)                  ->  表达式使用（即替换成对应表达式）
```

==========================================

API:
```
class RegexResult:



Bool() -> bool

Group() -> 匹配到的字符串数组

Group(index) -> 匿名捕获的字符串数组

Group(name)  -> 命名捕获的字符串数组





class Regex:



Match(testString, beginIndex=0) -> RegexResult

Search(testString, beginIndex=0) -> RegexResult

FindAll(testString, beginIndex=0) -> RegexResult

Replace(testString, replaceStr, replaceTime=1, beginIndex=0) -> resultString

ReplaceAll(testString, replaceStr, beginIndex=0) -> resultString
```


============================================



examples:
```
auto rst = Regex("^.+?@(?<type>.+?)\\.(?(com|cn))$").Match("563748559@qq.com");

rst.Group().size() == 1;

rst.Group()[0] == "563748559@qq.com";

rst.Group(1).size() == 1;

rst.Group(1)[0] == "com";

rst.Group("type").size() == 1;

rst.Group("type")[0] == "563748559";
```


more examples in source/test/test.cpp




/*
 @licstart  The following is the entire license notice for the JavaScript code in this file.

 The MIT License (MIT)

 Copyright (C) 1997-2020 by Dimitri van Heesch

 Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 and associated documentation files (the "Software"), to deal in the Software without restriction,
 including without limitation the rights to use, copy, modify, merge, publish, distribute,
 sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all copies or
 substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

 @licend  The above is the entire license notice for the JavaScript code in this file
*/
var NAVTREE =
[
  [ "Koala", "index.html", [
    [ "Документация по core/async.h", "md_core_2async_8doc.html", [
      [ "Описание", "md_core_2async_8doc.html#autotoc_md1", null ],
      [ "Основные структуры", "md_core_2async_8doc.html#autotoc_md3", [
        [ "AsyncTask", "md_core_2async_8doc.html#autotoc_md4", null ],
        [ "AsyncTaskNode", "md_core_2async_8doc.html#autotoc_md5", null ],
        [ "AsyncEventLoop", "md_core_2async_8doc.html#autotoc_md6", null ]
      ] ],
      [ "Основные функции", "md_core_2async_8doc.html#autotoc_md8", [
        [ "AsyncEventLoop* async_event_loop_init();", "md_core_2async_8doc.html#autotoc_md9", null ],
        [ "void async_event_loop_stop(AsyncEventLoop* loop);", "md_core_2async_8doc.html#autotoc_md10", null ],
        [ "AsyncTask* async_run(AsyncEventLoop* loop, FunctionPipeline* func, FunctionParams* params);", "md_core_2async_8doc.html#autotoc_md11", null ],
        [ "Result async_await(AsyncTask* task);", "md_core_2async_8doc.html#autotoc_md12", null ]
      ] ],
      [ "Интеграция с FunctionPipeline и Result", "md_core_2async_8doc.html#autotoc_md14", null ],
      [ "Новые возможности (v2)", "md_core_2async_8doc.html#autotoc_md16", [
        [ "Пул потоков", "md_core_2async_8doc.html#autotoc_md17", null ],
        [ "Отмена задач", "md_core_2async_8doc.html#autotoc_md18", null ],
        [ "Таймауты", "md_core_2async_8doc.html#autotoc_md19", null ]
      ] ],
      [ "Примеры использования", "md_core_2async_8doc.html#autotoc_md21", [
        [ "Простой пример (C)", "md_core_2async_8doc.html#autotoc_md22", null ],
        [ "Пример с аргументами (C)", "md_core_2async_8doc.html#autotoc_md23", null ],
        [ "Пример с несколькими задачами (C++)", "md_core_2async_8doc.html#autotoc_md24", null ],
        [ "Пример обработки ошибок (C++)", "md_core_2async_8doc.html#autotoc_md25", null ],
        [ "Пул потоков (C++)", "md_core_2async_8doc.html#autotoc_md26", null ],
        [ "Ожидание с таймаутом (C++)", "md_core_2async_8doc.html#autotoc_md27", null ],
        [ "Отмена задачи (C++)", "md_core_2async_8doc.html#autotoc_md28", null ]
      ] ],
      [ "Потокобезопасность", "md_core_2async_8doc.html#autotoc_md30", null ],
      [ "Ограничения и замечания", "md_core_2async_8doc.html#autotoc_md31", null ],
      [ "Расширяемость", "md_core_2async_8doc.html#autotoc_md32", null ],
      [ "См. также", "md_core_2async_8doc.html#autotoc_md34", null ],
      [ "Иллюстрированные схемы", "md_core_2async_8doc.html#autotoc_md35", [
        [ "Архитектура Event Loop", "md_core_2async_8doc.html#autotoc_md36", null ],
        [ "Очередь задач (C++)", "md_core_2async_8doc.html#autotoc_md37", null ],
        [ "Взаимодействие async_run и async_await", "md_core_2async_8doc.html#autotoc_md38", null ],
        [ "Структура AsyncTask", "md_core_2async_8doc.html#autotoc_md39", null ],
        [ "Поток данных через Event Loop", "md_core_2async_8doc.html#autotoc_md40", null ]
      ] ],
      [ "Пояснения по новым полям структур", "md_core_2async_8doc.html#autotoc_md41", null ]
    ] ],
    [ "Документация по core/types.h", "md_core_2types_8doc.html", [
      [ "Описание", "md_core_2types_8doc.html#autotoc_md43", null ],
      [ "Основные типы", "md_core_2types_8doc.html#autotoc_md45", [
        [ "Целочисленные алиасы", "md_core_2types_8doc.html#autotoc_md46", null ],
        [ "Generic", "md_core_2types_8doc.html#autotoc_md47", [
          [ "Пример (C)", "md_core_2types_8doc.html#autotoc_md48", null ],
          [ "Пример (C++)", "md_core_2types_8doc.html#autotoc_md49", null ]
        ] ],
        [ "Result", "md_core_2types_8doc.html#autotoc_md50", [
          [ "Пример (C)", "md_core_2types_8doc.html#autotoc_md51", null ],
          [ "Пример (C++)", "md_core_2types_8doc.html#autotoc_md52", null ]
        ] ],
        [ "FunctionParams", "md_core_2types_8doc.html#autotoc_md53", [
          [ "Пример (C)", "md_core_2types_8doc.html#autotoc_md54", null ],
          [ "Пример (C++)", "md_core_2types_8doc.html#autotoc_md55", null ]
        ] ],
        [ "FunctionPipeline", "md_core_2types_8doc.html#autotoc_md56", [
          [ "Пример (C)", "md_core_2types_8doc.html#autotoc_md57", null ],
          [ "Пример (C++)", "md_core_2types_8doc.html#autotoc_md58", null ]
        ] ]
      ] ],
      [ "Интеграция с async.h", "md_core_2types_8doc.html#autotoc_md60", null ],
      [ "Рекомендации по памяти и безопасности", "md_core_2types_8doc.html#autotoc_md62", null ],
      [ "Расширяемость", "md_core_2types_8doc.html#autotoc_md64", null ],
      [ "Пример комплексного использования с async.h (C)", "md_core_2types_8doc.html#autotoc_md66", null ],
      [ "См. также", "md_core_2types_8doc.html#autotoc_md68", null ],
      [ "FAQ", "md_core_2types_8doc.html#autotoc_md70", null ],
      [ "Иллюстрированные схемы", "md_core_2types_8doc.html#autotoc_md72", [
        [ "Generic (C)", "md_core_2types_8doc.html#autotoc_md73", null ],
        [ "Generic<T> (C++)", "md_core_2types_8doc.html#autotoc_md74", null ],
        [ "Result", "md_core_2types_8doc.html#autotoc_md75", null ],
        [ "FunctionParams", "md_core_2types_8doc.html#autotoc_md76", null ],
        [ "FunctionPipeline (цепочка)", "md_core_2types_8doc.html#autotoc_md77", null ],
        [ "Поток данных через FunctionPipeline и async", "md_core_2types_8doc.html#autotoc_md78", null ]
      ] ],
      [ "Типичные ошибки и их решения", "md_core_2types_8doc.html#autotoc_md80", null ]
    ] ],
    [ "Topics", "topics.html", "topics" ],
    [ "Data Structures", "annotated.html", [
      [ "Data Structures", "annotated.html", "annotated_dup" ],
      [ "Data Structure Index", "classes.html", null ],
      [ "Data Fields", "functions.html", [
        [ "All", "functions.html", null ],
        [ "Variables", "functions_vars.html", null ]
      ] ]
    ] ],
    [ "Files", "files.html", [
      [ "File List", "files.html", "files_dup" ],
      [ "Globals", "globals.html", [
        [ "All", "globals.html", null ],
        [ "Functions", "globals_func.html", null ],
        [ "Typedefs", "globals_type.html", null ],
        [ "Enumerations", "globals_enum.html", null ],
        [ "Enumerator", "globals_eval.html", null ],
        [ "Macros", "globals_defs.html", null ]
      ] ]
    ] ]
  ] ]
];

var NAVTREEINDEX =
[
"annotated.html"
];

var SYNCONMSG = 'click to disable panel synchronization';
var SYNCOFFMSG = 'click to enable panel synchronization';
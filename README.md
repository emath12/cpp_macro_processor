# C++ Macro_Processor [In Progress]

This is a LaTeX inspired macro processor written in C++. It allows you to map words to expansions.

::**CALLING A MACRO**::
Macros are defined, as in LaTeX, with a **'\'** character  

*Example*
\macro_name calls a macro named macro_name. 

**DEFINING A MACRO (\def)**
\def{macro_name}{expansion}
Defines macro_name to the requisite expansion.

Using the pre-defined "def" macro, you can define your own macros, referred to as **user macros**. 

*Example*
*::Input::*
*/def{hello}{bonjour}*
*/hello*

::*::Output::*
*bonjour*

**OTHER SUPPORTED PREDEFINED MACROS**

Here is a list of the other supported, pre-defined macros. This project is a work in progress, with more being added
as time goes on.

**\ifdef**

\ifdef{macro_name}{then}{else}

Only proceeds with the "else" portion if macro_name is not already. defined. Otherwise, it processes the "then" portion.

**\undef**

Allows you to undefine a macro. Note, redefining macros **is** legal (i.e., using \def on a macro that you know to already exist, which will simply redefine the macro to the new value), so you need not use this first to undefine, then redefine.

*Example*

*INPUT*
*\def{day}{jour}*
*\day*
*\undef{day}*
*\day*

*OUTPUT*
[Invalid Macro Call] "jour" is not defined

**\expandafter**

\expandafter{before}{after}

Expands "before", and then after "before" is done (successfully) expanding, "after" is expanded.












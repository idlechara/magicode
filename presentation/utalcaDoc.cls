%
% Formato de carta, Universidad de Talca
% basado en la clase letter
% por Rodrigo Paredes, raparede@utalca.cl
% modificado por Erik Regla, eregla09@alumnos.utalca.cl
%

\NeedsTeXFormat{LaTeX2e}
\ProvidesClass{utalcaDoc}[2009/08/28 Utalca Article, based on the article class]

\DeclareOption*{\PassOptionsToClass{\CurrentOption}{article}}
\ProcessOptions\relax
\LoadClass{article}
\RequirePackage{epsf}

% page controls
\RequirePackage{fullpage}
%\addtolength{\voffset}{0cm} % limite superior
%\addtolength{\topmargin}{0cm} % espacio sobre header
\addtolength{\headheight}{0mm} % ancho header
\addtolength{\headsep}{10mm} % espacio bajo header
\addtolength{\textheight}{-5mm} % largo texto
%\addtolength{\hoffset}{0cm} % limite izquierdo
%\addtolength{\textwidth}{0cm} % ancho texto

%paragraph controls
\setlength{\parindent}{8mm}
\setlength{\parskip}{2mm}

\def\@utalcaHeadBody{%
  \vbox{%
    % ojo que las lineas se terminan con %, sino pasan cosas que aun no entiendo porque
    % \epsffile carga postscripts
    % \rlap genera un reglon en el encabezado
    % \kern controla posicion en la horizontal
    % \lower controla posicion en la vertical
    % \hbox define una caja de texto
    % con 4 lineas: vspace -20mm, y reglones en -12mm, -7mm, -2.8mm, -0mm
    % con 3 lineas: vspace -17mm y reglones en -9.5mm, -5mm, -1mm
    \vspace{-17mm}%
    %\hspace{-0.8cm}%
    \epsffile{utalca.eps} %{\epsfxsize16mm \epsffile{utalca.eps}} %
    \rlap{\kern 4mm \lower -9.5mm \hbox to0pt{{\Large \sc Universidad~~de~~Talca}\hfill}}%
    \rlap{\kern 4mm \lower -5mm \hbox to0pt{\Large \sc Facultad~~de~~Ingenier\'ia\hfill}}%
    \rlap{\kern 4mm \lower -1mm \hbox to0pt{%
      {\large \sc Departamento~~de~~Ciencias~~de~~la~~Computaci\'on}\hfill}}%
  }%
}

\def\@utalcaFootBody{%
}

\def\ps@firstpage{%
     \def\@oddhead{\@utalcaHeadBody}
     \def\@oddfoot{\@utalcaFootBody}}

%\renewcommand{\@oddhead}{\@utalcaHeadBody}
%\renewcommand{\@oddfoot}{\@utalcaFootBody}

\renewcommand\maketitle{\par
  \begingroup %se usa en vez de las llaves { } para evitar confusiones, cierra un \endgroup
    \renewcommand\thefootnote{\@fnsymbol\c@footnote}%
    \def\@makefnmark{\rlap{\@textsuperscript{\normalfont\@thefnmark}}}%
    \long\def\@makefntext##1{\parindent 1em\noindent
            \hb@xt@1.8em{%
                \hss\@textsuperscript{\normalfont\@thefnmark}}##1}%
    \if@twocolumn
      \ifnum \col@number=\@ne
        \@maketitle
      \else
        \twocolumn[\@maketitle]%
      \fi
    \else
      \newpage
      \global\@topnum\z@   % Prevents figures from going at top of page.
      \@maketitle
    \fi
    \thispagestyle{firstpage}\@thanks
  \endgroup
  \setcounter{footnote}{0}%
  \global\let\thanks\relax
  \global\let\maketitle\relax
  \global\let\@maketitle\relax
  \global\let\@thanks\@empty
  \global\let\@author\@empty
  \global\let\@date\@empty
  \global\let\@title\@empty
  \global\let\title\relax
  \global\let\author\relax
  \global\let\date\relax
  \global\let\and\relax
}


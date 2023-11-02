$$
\begin{align}
    [\text{prog}] &\to [\text{stmt}]^* \\
    [\text{stmt}] &\to 
    \begin{cases}
        \text{exit}([\text{expression}]); \\
        \text{var}\space\text{ident} = [\text{expression}];
    \end{cases}
    \\
    [\text{expression}] &\to 
    \begin{cases}
        \text{int\_lit} \\
        \text{ident}
    \end{cases}
\end{align}
$$
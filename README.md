- faire struct token pour chaque token liste
- IO backend : input char par char
- Lexer : renvoie 1 token par appel
- Parser : appeler recurssivement avec un algo de lookahead des tokens du lexer et verifie s'il verifie la grammaire
- Parse execute loop : appeler parser qui va renvoyer la head de l'ast

Questions :
- comprendre comment lier les noeuds de l'ast (a la remonte ? / trimballer le pere)
- \n et eof sont des tokens ??
-


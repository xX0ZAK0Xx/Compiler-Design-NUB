#include <bits/stdc++.h>
using namespace std;

void analyzer(string input, regex rules, string token){
  smatch matches;
  vector<string> whatever; int ctr = 0;
  while(regex_search(input, matches, rules)){
    whatever.push_back(matches[0]);
    ctr++;
    input = matches.suffix().str();
  }

  cout<<token<<" ("<<ctr<<")\t: ";
  for(int i=0; i<whatever.size(); i++){
    cout<<whatever[i]<<" ";
  }
  cout<<endl;
  return;
}

int main(){
  string input;
  input = "float a = (b*c) + a1  % 10  / int(-3.1416);";

  regex constantRegex("(\\+|\\-)?\\b\\d+(\\.\\d+)?\\b");
  regex keywordRegex("\\b(auto|break|case|class|catch|const|continue|default|do|double|else|enum|explicit|export|extern|float|for|friend|goto|if|inline|int|long|namespace|new|operator|private|protected|public|register|return|short|signed|sizeof|static|string|struct|switch|template|this|throw|typedef|union|unsigned|using|virtual|void|volatile|cout|endl|while)\\b");
  regex operatorRegex("(\\+|-|\\*|/|%|==|=|!=|<=|>=|<|>|<<|>>)+(?!\\d)");
  regex punctuationRegex("[#$',:;?@^_`|~]");
  regex parenthesisRegex("[()|{}|\\[\\]]");
  regex identifierRegex("\\b(?!auto|break|case|class|catch|const|continue|default|do|double|else|enum|explicit|export|extern|float|for|friend|goto|if|inline|int|long|namespace|new|operator|private|protected|public|register|return|short|signed|sizeof|static|string|struct|switch|template|this|throw|typedef|union|unsigned|using|virtual|void|volatile|while)\\b[a-zA-Z_]+[a-zA-Z0-9_]*");
  
  analyzer(input, keywordRegex, "Keyword");
  analyzer(input, identifierRegex, "Identifier");
  analyzer(input, operatorRegex, "Operator");
  analyzer(input, constantRegex, "Constant");
  analyzer(input, punctuationRegex, "Punctuation");
  analyzer(input, parenthesisRegex, "Parenthesis");

  return 0;
}

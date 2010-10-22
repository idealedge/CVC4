/*********************                                                        */
/*! \file interactive_shell.cpp
 ** \verbatim
 ** Original author: cconway
 ** Major contributors: 
 ** Minor contributors (to current version): 
 ** This file is part of the CVC4 prototype.
 ** Copyright (c) 2009, 2010  The Analysis of Computer Systems Group (ACSys)
 ** Courant Institute of Mathematical Sciences
 ** New York University
 ** See the file COPYING in the top-level source directory for licensing
 ** information.\endverbatim
 **
 ** \brief Interactive shell for CVC4
 **/

#include <iostream>

#include "interactive_shell.h"
#include "expr/command.h"
#include "parser/parser.h"
#include "parser/parser_builder.h"

using namespace std;

namespace CVC4 {

Command* InteractiveShell::readCommand() {
  /* Don't do anything if the input is closed. */
  if( d_in.eof() ) {
    return NULL;
  }

  /* If something's wrong with the input, there's nothing we can do. */
  if( !d_in.good() ) {
    throw ParserException("Interactive input broken.");
  }

  string input;

  /* Prompt the user for input. */
  d_out << "CVC4> " << flush;
  while(true) {
    stringbuf sb;
    string line;

    /* Read a line */
    d_in.get(sb,'\n');
    line = sb.str();
    // cout << "Input was '" << input << "'" << endl << flush;

    /* If we hit EOF, we're done. */
    if( d_in.eof() ) {
      input += line;
      break;
    }

    /* Check for failure */
    if( d_in.fail() ) {
      /* This should only happen if the input line was empty. */
      Assert( line.empty() );
      d_in.clear();
    }

    /* Extract the newline delimiter from the stream too */
    int c = d_in.get();
    Assert( c == '\n' );

    // cout << "Next char is '" << (char)c << "'" << endl << flush;

    /* Strip trailing whitespace. */
    int n = line.length() - 1;
    while( !line.empty() && isspace(line[n]) ) { 
      line.erase(n,1); 
      n--;
    }

    input += line;
    
    /* If the last char was a backslash, continue on the next line. */
    if( !line.empty() && line[n] == '\\' ) {
      n = input.length() - 1;
      Assert( input[n] == '\\' );
      input[n] = '\n';
      d_out << "... > " << flush;
    } else {
      /* No continuation, we're done. */
      //cout << "Leaving input loop." << endl << flush;
      break;
    }
  }

  Parser *parser = 
    d_parserBuilder
        .withStringInput(input)
        .withStateFrom(d_lastParser)
        .build();

  /* There may be more than one command in the input. Build up a
     sequence. */
  CommandSequence *cmd_seq = new CommandSequence();
  Command *cmd;

  while( (cmd = parser->nextCommand()) ) {
    cmd_seq->addCommand(cmd);
  }

  delete d_lastParser;
  d_lastParser = parser;

  return cmd_seq;
}

} // CVC4 namespace

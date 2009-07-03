
// Copyright (c) 1999-2006 by Digital Mars
// All Rights Reserved
// written by Walter Bright
// www.digitalmars.com
// License for redistribution is by either the Artistic License
// in artistic.txt, or the GNU General Public License in gnu.txt.
// See the included readme.txt for details.

#include <stdio.h>
#include <assert.h>

#include "mars.h"
#include "expression.h"
#include "mtype.h"
#include "utf.h"
#include "declaration.h"
#include "aggregate.h"

/********************************************
 * Convert from expression to delegate that returns the expression,
 * i.e. convert:
 *	expr
 * to:
 *	t delegate() { return expr; }
 */

Expression *Expression::toDelegate(Scope *sc, Type *t)
{
    //printf("Expression::toDelegate(t = %s)\n", t->toChars());
    TypeFunction *tf = new TypeFunction(NULL, t, 0, LINKd);
    FuncLiteralDeclaration *fld =
	new FuncLiteralDeclaration(loc, loc, tf, TOKdelegate, NULL);
    Expression *e;
#if 1
    sc = sc->push();
    sc->parent = fld;		// set current function to be the delegate
    e = this;
    e->scanForNestedRef(sc);
    sc = sc->pop();
#else
    e = this->syntaxCopy();
#endif
    Statement *s = new ReturnStatement(loc, e);
    fld->fbody = s;
    e = new FuncExp(loc, fld);
    e = e->semantic(sc);
    return e;
}

/******************************
 * Perform scanForNestedRef() on an array of Expressions.
 */

void arrayExpressionScanForNestedRef(Scope *sc, Expressions *a)
{
    //printf("arrayExpressionScanForNestedRef(%p)\n", a);
    if (a)
    {
	for (int i = 0; i < a->dim; i++)
	{   Expression *e = (Expression *)a->data[i];

	    if (e)
	    {
		e->scanForNestedRef(sc);
	    }
	}
    }
}

void Expression::scanForNestedRef(Scope *sc)
{
}

void SymOffExp::scanForNestedRef(Scope *sc)
{
    //printf("SymOffExp::scanForNestedRef(%s)\n", toChars());
    VarDeclaration *v = var->isVarDeclaration();
    if (v)
	v->checkNestedReference(sc, 0);
}

void VarExp::scanForNestedRef(Scope *sc)
{
    //printf("VarExp::scanForNestedRef(%s)\n", toChars());
    VarDeclaration *v = var->isVarDeclaration();
    if (v)
	v->checkNestedReference(sc, 0);
}

void ThisExp::scanForNestedRef(Scope *sc)
{
    assert(var);
    var->isVarDeclaration()->checkNestedReference(sc, 0);
}

void SuperExp::scanForNestedRef(Scope *sc)
{
    assert(0);
}

void DeclarationExp::scanForNestedRef(Scope *sc)
{
    printf("DeclarationExp::scanForNestedRef() %s\n", toChars());
    //assert(0);
}

void NewExp::scanForNestedRef(Scope *sc)
{
    //printf("NewExp::scanForNestedRef(Scope *sc): %s\n", toChars());

    if (thisexp)
	thisexp->scanForNestedRef(sc);
    arrayExpressionScanForNestedRef(sc, newargs);
    arrayExpressionScanForNestedRef(sc, arguments);
}

void UnaExp::scanForNestedRef(Scope *sc)
{
    e1->scanForNestedRef(sc);
}

void BinExp::scanForNestedRef(Scope *sc)
{
    e1->scanForNestedRef(sc);
    e2->scanForNestedRef(sc);
}

void CallExp::scanForNestedRef(Scope *sc)
{
    //printf("CallExp::scanForNestedRef(Scope *sc): %s\n", toChars());
    e1->scanForNestedRef(sc);
    arrayExpressionScanForNestedRef(sc, arguments);
}


void IndexExp::scanForNestedRef(Scope *sc)
{
    e1->scanForNestedRef(sc);

    if (lengthVar)
    {	//printf("lengthVar\n");
	lengthVar->parent = sc->parent;
    }
    e2->scanForNestedRef(sc);
}


void SliceExp::scanForNestedRef(Scope *sc)
{
    e1->scanForNestedRef(sc);

    if (lengthVar)
    {	//printf("lengthVar\n");
	lengthVar->parent = sc->parent;
    }
    if (lwr)
	lwr->scanForNestedRef(sc);
    if (upr)
	upr->scanForNestedRef(sc);
}


void ArrayLiteralExp::scanForNestedRef(Scope *sc)
{
    arrayExpressionScanForNestedRef(sc, elements);
}


void ArrayExp::scanForNestedRef(Scope *sc)
{
    e1->scanForNestedRef(sc);
    arrayExpressionScanForNestedRef(sc, arguments);
}


void CondExp::scanForNestedRef(Scope *sc)
{
    econd->scanForNestedRef(sc);
    e1->scanForNestedRef(sc);
    e2->scanForNestedRef(sc);
}




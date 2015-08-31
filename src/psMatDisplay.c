/*
Copyright (c) 1990 Massachusetts Institute of Technology, Cambridge, MA.
All rights reserved.

This Agreement gives you, the LICENSEE, certain rights and obligations.
By using the software, you indicate that you have read, understood, and
will comply with the terms.

Permission to use, copy and modify for internal, noncommercial purposes
is hereby granted.  Any distribution of this program or any part thereof
is strictly prohibited without prior written consent of M.I.T.

Title to copyright to this software and to any associated documentation
shall at all times remain with M.I.T. and LICENSEE agrees to preserve
same.  LICENSEE agrees not to make any copies except for LICENSEE'S
internal noncommercial use, or to use separately any portion of this
software without prior written consent of M.I.T.  LICENSEE agrees to
place the appropriate copyright notice on any such copies.

Nothing in this Agreement shall be construed as conferring rights to use
in advertising, publicity or otherwise any trademark or the name of
"Massachusetts Institute of Technology" or "M.I.T."

M.I.T. MAKES NO REPRESENTATIONS OR WARRANTIES, EXPRESS OR IMPLIED.  By
way of example, but not limitation, M.I.T. MAKES NO REPRESENTATIONS OR
WARRANTIES OF MERCHANTABILITY OR FITNESS FOR ANY PARTICULAR PURPOSE OR
THAT THE USE OF THE LICENSED SOFTWARE COMPONENTS OR DOCUMENTATION WILL
NOT INFRINGE ANY PATENTS, COPYRIGHTS, TRADEMARKS OR OTHER RIGHTS.
M.I.T. shall not be held liable for any liability nor for any direct,
indirect or consequential damages with respect to any claim by LICENSEE
or any third party on account of or arising from this Agreement or use
of this software.
*/

#include "mulGlobal.h"
#include "zbufGlobal.h"

/*
  for writting sparsity pattern ps file in Aldus Freehand format
*/

/* must be defined somewhere */
#ifndef OPEN
#define OPEN 0			/* open ps file, print hdr, ignore row/col */
#endif
#ifndef CLOSE
#define CLOSE 1			/* print trailer, close ps file */
#endif
#ifndef UPDATE
#define UPDATE 2		/* => add 2 dots for this row and col */
#endif
#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif
#ifndef MIN
#define MIN(A,B)  ( (A) > (B) ? (B) : (A) )
#endif


#define FONT 5.0                /* font size used for labels in ps file */
#define CMDFONT 10.0            /* font used to write command line */

#define OFFSETX 34.0            /* offset on x from lower left (pnts) */
#define OFFSETY 34.0            /* offset on y from lower left (pnts) */
#define IMAGEX 540.0            /* x height of image (chosen to be points) */
#define IMAGEY 720.0            /* y height of image (chosen to be points)
                                   command line scale parameter muliplies
                                   IMAGEX/Y to get final image size
                                   - these values should allow 7.5x10 images */

/* SRW -- Now including zbufGlobal.h, override these. */
#undef LINCAP
#undef LINJIN

#define LINCAP 0		/* line cap parameter - 0 => butt cap */
#define LINJIN 0		/* line join parameter - 0 => miter join */
#define GREYLV 0		/* grey level for lines - 0 => black */

#define LINPIN 300		/* lines per inch resolution of printer */
#define MINLIN 5		/* min number of lines per aliased row/col */
#define MKSPNT (LINPIN/(MINLIN*72.0)) /* #marks per point - intermediate val */
#define ASIZER floor(IMAGEY*MKSPNT)/* matrix aliased down to this many rows */
#define ASIZEC floor(IMAGEX*MKSPNT)/* matrix aliased down to this many cols */

char **aliased_matrix;	/* the aliased matrix */

/* SRW */
void dump_aldus_hdr(FILE*, double, double, double, double, int);
void dump_aldus_foot(FILE*, int, char**, int, int, char*);
void dump_block(FILE*, double, double, double, double);
void dump_line(FILE*, double, double, double);
void dump_ps_mat(char*, int, int, int, int, char**, int, int);


/*
  dump Aldus Freehand readable header
*/
void dump_aldus_hdr(FILE *fp, double lowx, double lowy, double upx, double upy,
    int dump_text_hdr)
{
  /* print the lines before the bounding box */
  fprintf(fp, "%%!PS-Adobe-2.0 EPSF-1.2\n");
  fprintf(fp, "%%%%Creator: FreeHand\n");
  fprintf(fp, "%%%%Title: test.ps\n");
  fprintf(fp, "%%%%CreationDate: 4/19/90 10:47 AM\n");
  fprintf(fp, "%%%%BoundingBox: %g %g %g %g\n", lowx, lowy, upx, upy);

  copyBody(fp);                 /* copys the body of the header from
                                   "afhpsheader" */

  /* dump the text header if needed */
  if(dump_text_hdr) {
    fprintf(fp, "/textopf false def\n/curtextmtx{}def\n/otw .25 def\n");
    fprintf(fp, "/msf{dup/curtextmtx xdf makefont setfont}bdf\n");
    fprintf(fp, "/makesetfont/msf load def\n");
    fprintf(fp, "/curtextheight{.707104 .707104 curtextmtx dtransform\n");
    fprintf(fp, "dup mul exch dup mul add sqrt}bdf\n");
    fprintf(fp, "/ta{1 index\n{tempstr 0 2 index put tempstr 2 index\n");
    fprintf(fp, "gsave exec grestore\ntempstr stringwidth rmoveto\n");
    fprintf(fp, "5 index eq{6 index 6 index rmoveto}if\n");
    fprintf(fp, "3 index 3 index rmoveto\n");
    fprintf(fp, "}forall 7{pop}repeat}bdf\n");
    fprintf(fp,
            "/sts{setcolor textopf setoverprint/ts{awidthshow}def exec}bdf\n");
    fprintf(fp, "/stol{setlinewidth setcolor textopf setoverprint newpath\n");
    fprintf(fp, "/ts{{false charpath stroke}ta}def exec}bdf\n");
  }

  /* print rest of header (starting with after /currentpacking where...) */
  fprintf(fp, "/currentpacking where{pop false setpacking}if\n");
  fprintf(fp, "%%%%EndSetup\n");
  fprintf(fp, "/spots[1 0 0 0 (Process Cyan) false newcmykcustomcolor\n");
  fprintf(fp, "0 1 0 0 (Process Magenta) false newcmykcustomcolor\n");
  fprintf(fp, "0 0 1 0 (Process Yellow) false newcmykcustomcolor\n");
  fprintf(fp,
          "0 0 0 1 (Process Black) false newcmykcustomcolor\n]def\nvms\n");

}

/*
  dump Aldus Freehand readable trailer
*/
void dump_aldus_foot(FILE *fp, int insert_showpage, char **argv, int argc,
    int print_cmd_line, char *aux_str)
{
  int f;
  char line[BUFSIZ];

  /* print footer */
  if(print_cmd_line) {                      /* print cmnd line if asked for */
    for(f = 0, line[0] = '\0'; f < argc; f++) {
      strcat(line, argv[f]);
      strcat(line, " ");
    }
    strcat(line, aux_str);
    dump_line_as_ps(fp, line, OFFSETX+2*CMDFONT, OFFSETY+IMAGEY+CMDFONT/2, 
		    CMDFONT);
    /* fprintf(stderr, "Command line printed\n"); */
  }

  fprintf(fp, "vmr\nend  %% FreeHandDict\n");
  if(insert_showpage) {
    fprintf(fp, "showpage\n");
    /* fprintf(stderr, "Showpage inserted\n"); */
  }
  fprintf(fp, "%%%%EndDocument: _\n");

}

/*
  dumps a black filled block xwid wide by ywid tall, 
    with lower left corner (xstart, ystart)
*/
void dump_block(FILE *fp, double xstart, double ystart, double xwid,
    double ywid)
{
  fprintf(fp, "newpath\n");
  fprintf(fp, "%g %g moveto\n", xstart, ystart);
  fprintf(fp, "%g %g lineto\n", xstart+xwid, ystart); /* over, to right */
  fprintf(fp, "%g %g lineto\n", xstart+xwid, ystart+ywid); /* up */
  fprintf(fp, "%g %g lineto\n", xstart, ystart+ywid); /* over, to left */
  fprintf(fp, "closepath\n");
  fprintf(fp, " 0 setgray fill\n");
}

/*
  dumps a line of length len with left center (xstart, ystart)
  assumes line width, grey level and line ends set up before
*/
void dump_line(FILE *fp, double xstart, double ystart, double len)
{
  /*fprintf(fp, "%g %g moveto ", xstart, ystart);
  fprintf(fp, "%g %g lineto stroke\n", xstart+len, ystart);*/
  fprintf(fp, "%g %g L\n", xstart, ystart);
}

/* 
  dumps black squares for entries of P done directly in ps format
  - scales to fit on image size region given by above #define's
  - prints command line args on output (also what aliasing was done)
  - call first with type = OPEN, then on each calcp with type = UPDATE,
    finally with type = CLOSE
*/
void dump_ps_mat(char *filename, int row, int col, int num_row, int num_col,
    char **argv, int argc, int type)
{
  static FILE *fp = NULL;
  double widx, widy;
  double blk_size_x, blk_size_y;
  static double blk_size;

  static int alias_size_r, alias_size_c, alias_size;
  static int num_alias_r, num_alias_c;
  static int rows_aliased = FALSE;
  static int cols_aliased = TRUE;
  static int rows, cols;
  int i, j;
  char str[BUFSIZ];

  if(type == OPEN) {
    rows = num_row;
    cols = num_col;

    if((fp = fopen(filename, "w")) == NULL) {
      fprintf(stderr, "dump_ps_mat: can't open `%s' to write\n", filename);
      exit(1);
    }

    /* if size of matrix is less than alias limits in both dimensions
       do no aliasing */
    if(num_col <= (int)ASIZEC && num_row <= (int)ASIZER) {
      /* figure block size - adjust so constrained dimension is filled */
      blk_size_x = IMAGEX/(double)num_col;
      blk_size_y = IMAGEY/(double)num_row;
      num_alias_r = num_row;
      num_alias_c = num_col;
    }
    else {			/* alias in x or y if needed */
      if(num_row > (int)ASIZER) {
	rows_aliased = TRUE;
	/* get number of rows per cell (last cell may have different num) */
	/* - +1 needed to get even division case to work correctly */
	alias_size_r = (int) ceil((double)(num_row+1)/ASIZER);
      }
      else alias_size_r = 1;

      if(num_col > (int)ASIZEC) {
	cols_aliased = TRUE;
	/* get number of cols per cell (last cell may have different num) */
	/* - +1 needed to get even division case to work correctly */
	alias_size_c = (int) ceil((double)(num_col+1)/ASIZEC);
      }
      else alias_size_c = 1;

      /* use same alias size in both dimensions to keep aspect ratio of mtrx */
      alias_size = MAX(alias_size_r, alias_size_c);

      /* if either dimension is aliased, make sure the other one is too */
      if(rows_aliased && !cols_aliased) cols_aliased = TRUE;
      else if(!rows_aliased && cols_aliased) rows_aliased = TRUE;

      if(rows_aliased)	
	  num_alias_r = (int) ceil((double)num_row/(double)alias_size);
      else num_alias_r = num_row;

      if(cols_aliased) 
	  num_alias_c = (int) ceil((double)num_col/(double)alias_size);
      else num_alias_c = num_col;

    }

    /* allocate/initialize aliased matrix (may not actually be aliased) */
    CALLOC(aliased_matrix, num_alias_r, char*, ON, AMSC);
    for(i = 0; i < num_alias_r; i++) {
      CALLOC(aliased_matrix[i], num_alias_c, char, ON, AMSC);
      for(j = 0; j < num_alias_c; j++) aliased_matrix[i][j] = ' ';
    }

    /* figure true image size and dump header */
    blk_size_x = IMAGEX/(double)num_alias_c;
    blk_size_y = IMAGEY/(double)num_alias_r;
    blk_size = MIN(blk_size_x, blk_size_y);
    widx = blk_size * (double)num_alias_c;
    widy = blk_size * (double)num_alias_r;
    dump_aldus_hdr(fp, OFFSETX, OFFSETY, OFFSETX+widx, OFFSETY+widy, TRUE);

    /* setup lines to be of the proper size and shape */
    fprintf(fp, "%g setlinewidth %d setlinecap %d setlinejoin %d setgray\n",
	    blk_size, LINCAP, LINJIN, GREYLV);

    /* dump the line definition */
    fprintf(fp, "/L {2 copy moveto exch %g add exch lineto stroke} def\n",
	    blk_size);

  }
  else if(type == UPDATE) {
    if(fp == NULL) {
      fprintf(stderr, "dump_ps_mat: ps file not open\n");
      exit(1);
    }

    /* write a block in the (row, col) position
    dump_line(fp, OFFSETX+(blk_size*(double)col), 
	      OFFSETY+IMAGEY-(blk_size*(double)row)-blk_size/2.0, blk_size); */

    /* set the corresponding flag in the aliased matrix
       - floor of index divided by alias size */
    if(rows_aliased) i = (int) floor((double)(row-1)/(double)alias_size);
    else i = row-1;
    if(cols_aliased) j = (int) floor((double)(col-1)/(double)alias_size);
    else j = col-1;
    
    aliased_matrix[i][j] = '1';

  }
  else if(type == CLOSE) {
    /* dump the '1' entries of the aliased matrix
       - would be more efficient to dump lines of chars but blocks look nice 
       (at least for lower resolution) */
    for(i = 0; i < num_alias_r; i++) {
      for(j = 0; j < num_alias_c; j++) {
	if(aliased_matrix[i][j] == '1') {
	  dump_line(fp, OFFSETX+(blk_size*(double)j), 
		    OFFSETY+IMAGEY-(blk_size*(double)i)-blk_size/2.0, 
		    blk_size);
	}
      }
    }

    /* set up alias info string */
    sprintf(str, "     (matrix %dX%d, printed as %dX%d)",
	    rows, cols, num_alias_r, num_alias_c);

    dump_aldus_foot(fp, TRUE, argv, argc, TRUE, str);
    fclose(fp);
  }
  else {
    fprintf(stderr, "dump_ps_mat: bad type of call\n");
    exit(1);
  }
  
}

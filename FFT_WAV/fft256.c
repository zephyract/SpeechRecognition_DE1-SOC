/* fft256.c  complex data stored  re, im, re, im, re, ... im */

void fft256(float Z[512]) /* input data points and output  [0] to [511] */
{
  static float W[512];    /* scratch vector, used many times, */
  static float E[258] =   /* constants for FFT algorithm */ 
  {1.0, 0.0, 0.999699, 0.0245412,
   0.998796, 0.0490677, 0.99729, 0.0735646,
   0.995185, 0.0980171, 0.99248, 0.122411,
   0.989177, 0.14673, 0.985278, 0.170962,
   0.980785, 0.19509, 0.975702, 0.219101,
   0.970031, 0.24298, 0.963776, 0.266713,
   0.95694, 0.290285, 0.949528, 0.313682,
   0.941544, 0.33689, 0.932993, 0.359895,
   0.92388, 0.382683, 0.91421, 0.405241,
   0.903989, 0.427555, 0.893224, 0.449611,
   0.881921, 0.471397, 0.870087, 0.492898,
   0.857729, 0.514103, 0.844854, 0.534998,
   0.83147, 0.55557, 0.817585, 0.575808,
   0.803208, 0.595699, 0.788346, 0.615232,
   0.77301, 0.634393, 0.757209, 0.653173,
   0.740951, 0.671559, 0.724247, 0.689541,
   0.707107, 0.707107, 0.689541, 0.724247,
   0.671559, 0.740951, 0.653173, 0.757209,
   0.634393, 0.77301, 0.615232, 0.788346,
   0.595699, 0.803208, 0.575808, 0.817585,
   0.55557, 0.83147, 0.534998, 0.844854,
   0.514103, 0.857729, 0.492898, 0.870087,
   0.471397, 0.881921, 0.449611, 0.893224,
   0.427555, 0.903989, 0.405241, 0.91421,
   0.382683, 0.92388, 0.359895, 0.932993,
   0.33689, 0.941544, 0.313682, 0.949528,
   0.290285, 0.95694, 0.266713, 0.963776,
   0.24298, 0.970031, 0.219101, 0.975702,
   0.19509, 0.980785, 0.170962, 0.985278,
   0.14673, 0.989177, 0.122411, 0.99248,
   0.0980171, 0.995185, 0.0735646, 0.99729,
   0.0490677, 0.998796, 0.0245412, 0.999699,
   0.0, 1.0, -0.0245412, 0.999699,
   -0.0490677, 0.998796, -0.0735646, 0.99729,
   -0.0980171, 0.995185, -0.122411, 0.99248,
   -0.14673, 0.989177, -0.170962, 0.985278,
   -0.19509, 0.980785, -0.219101, 0.975702,
   -0.24298, 0.970031, -0.266713, 0.963776,
   -0.290285, 0.95694, -0.313682, 0.949528,
   -0.33689, 0.941544, -0.359895, 0.932993,
   -0.382683, 0.92388, -0.405241, 0.91421,
   -0.427555, 0.903989, -0.449611, 0.893224,
   -0.471397, 0.881921, -0.492898, 0.870087,
   -0.514103, 0.857729, -0.534998, 0.844854,
   -0.55557, 0.83147, -0.575808, 0.817585,
   -0.595699, 0.803208, -0.615232, 0.788346,
   -0.634393, 0.77301, -0.653173, 0.757209,
   -0.671559, 0.740951, -0.689541, 0.724247,
   -0.707107, 0.707107, -0.724247, 0.689541,
   -0.740951, 0.671559, -0.757209, 0.653173,
   -0.77301, 0.634393, -0.788346, 0.615232,
   -0.803208, 0.595699, -0.817585, 0.575808,
   -0.83147, 0.55557, -0.844854, 0.534998,
   -0.857729, 0.514103, -0.870087, 0.492898,
   -0.881921, 0.471397, -0.893224, 0.449611,
   -0.903989, 0.427555, -0.91421, 0.405241,
   -0.92388, 0.382683, -0.932993, 0.359895,
   -0.941544, 0.33689, -0.949528, 0.313682,
   -0.95694, 0.290285, -0.963776, 0.266713,
   -0.970031, 0.24298, -0.975702, 0.219101,
   -0.980785, 0.19509, -0.985278, 0.170962,
   -0.989177, 0.14673, -0.99248, 0.122411,
   -0.995185, 0.0980171, -0.99729, 0.0735646,
   -0.998796, 0.0490677, -0.999699, 0.0245412,
   -1.0, 0.0};
  float Tre, Tim;
  int i, j, k, l, m;

   m = 128;
   l = 1;
   while(1)
   { 
      k = 0;
      j = l;
      i = 0;
      while(1)
      { 
         while(1)
         {
	   /* W[i+k] = Z[i] + Z[m+i]; complex */
           W[2*(i+k)]   = Z[2*i]   + Z[2*(m+i)];
           W[2*(i+k)+1] = Z[2*i+1] + Z[2*(m+i)+1];

           /* W[i+j] = E[k] * (Z[i] - Z[m+i]); complex */
           Tre = Z[2*i]   - Z[2*(m+i)];
           Tim = Z[2*i+1] - Z[2*(m+i)+1];
           W[2*(i+j)]   = E[2*k] * Tre - E[2*k+1] * Tim;
           W[2*(i+j)+1] = E[2*k] * Tim + E[2*k+1] * Tre; 
           i++;
           if(i >= j) break;
	 }
         k = j;
         j = k+l;
         if(j > m) break;
      }
      l = l+l;
                  /* work back other way without copying */
      k = 0;
      j = l;
      i = 0;
      while(1)
      {
        while(1)
	{
          /* Z[i+k] = W[i] + W[m+i]; complex */
          Z[2*(i+k)]   = W[2*i]   + W[2*(m+i)];
          Z[2*(i+k)+1] = W[2*i+1] + W[2*(m+i)+1];

          /* Z[i+j] = E[k] * (W[i] - W[m+i]); complex */
          Tre = W[2*i]   - W[2*(m+i)];
          Tim = W[2*i+1] - W[2*(m+i)+1];
          Z[2*(i+j)]   = E[2*k] * Tre - E[2*k+1] * Tim;
          Z[2*(i+j)+1] = E[2*k] * Tim + E[2*k+1] * Tre;
          i++;
          if(i >= j) break;
	}
        k = j;
        j = k+l;
        if(j > m) break;
      }
      l = l+l;
      if(l > m) break; // result is in Z
   }
} /* end fft256 */


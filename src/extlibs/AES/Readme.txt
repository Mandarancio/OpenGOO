AES Code by Chris Lomont (chris@lomont.org), copyright 2000-2005.

Read the header AES.h to see how to use.

Due to recent interest in the code, I upgraded it a bit to make it easier to use.
In the past five years there have been no bugs reported. I hope this continues :)

If you use this code, please email me with comments and suggestions.

VERSION HISTORY

1.0 - Jun 2005 
      1. Fixed use of NULL in headers without being included.
      2. Added overall testing result at bottom of main.cpp.
      3. Added a few static_cast to remove all compile warnings. Changed
         all old style casts to new style casts.
      4. Changed encrypt/decrypt interface to make it easier to use by 
         renaming the old block based functions, and adding new functions
         that encrypt/decrypt multiple blocks.
      5. Input and output changed to unsigned char * from unsigned long * 
         to make easier to use.
      6. Removed the requirement to call CreateAESTables by making the 
         first constructed object create the internal tables.
      7. Added some const correctness.
      8. Default AES blocksize now 128 as per the standard. However, the
         code still allows 192 and 256 bit blocksizes.
      9. Added some padding tests to catch buffer overruns in the test file.
     10. Added random testing of random size data blocks.
     11. Replaced local items using static linkage to anonymous namespace
         since this static usage is deprecated.
     12. Implemented the multiple block encryption modes ECB and CBC. Plan
         to add CFB, OFB, CTR, EAX, GCM.
     13. Changed the project file to Visual C++.NET 2003.
         
0.9 - Nov 2000 - Initial release.

END OF FILE
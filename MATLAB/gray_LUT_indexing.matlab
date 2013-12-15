function [col_lut] = getColumnLUT(v_projected)
	%each gray value is stored at G+1, so a gray value of 0 (black) is in col_lut(1).
	col_lut = zeros(1, 256); 
	
	column = 0;

	gray = 10000;

	for x = 1 : size(v_projected, 2)
		%if this is a different column from the previous one, 
		%increase new column value and store in the gray value's index
		if(v_projected(1, x) != gray)
			column = column + 1;
			gray = v_projected(1, x);
			col_lut(gray) = column;
		end
	end
end

function [row_lut] = getRowLUT(h_projected)
	%each gray value is stored at G+1, so a gray value of 0 (black) is in row_lut(1).
	row_lut = zeros(1, 256); 
	
	row = 0;

	gray = 10000;

	for y = 1 : size(h_projected, 1)
		%if this is a different row from the previous one, 
		%increase new row value and store in the gray value's index
		if(h_projected(y, 1) != gray)
			row = row + 1;
			gray = h_projected(y, 1);
			row_lut(gray) = row;
		end
	end
end